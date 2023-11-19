#include "reassembler.hh"

#include <cstdint>

#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

Reassembler::Reassembler()
  : first_unpopped_(0), first_unassembled_(0), capacity_(0), buf_(),
    last_substring_arrived_(false), last_substring_end_(0) {}

void
Reassembler::try_insert(Writer &output)
{
  if (this->first_unassembled_ >= this->buf_.size()) {
    return;
  }

  if (std::nullopt == this->buf_[this->first_unassembled_]) {
    return;
  }

  for (std::size_t i = this->first_unassembled_;
       i < this->buf_.size() && this->buf_[i] != std::nullopt;
       i += 1)
  {
    output.push(std::string(1, this->buf_[i].value()));
    this->first_unassembled_ += 1;
  }
}

void
Reassembler::insert(std::uint64_t first_index, std::string data,
                    bool is_last_substring, Writer& output)
{
  if (output.is_closed()) {
    return;
  }

  if (is_last_substring) {
    this->last_substring_arrived_ = true;
    this->last_substring_end_ = first_index + data.length();
  }

  this->first_unpopped_ = output.reader().bytes_popped();
  this->capacity_ = this->first_unassembled_ - this->first_unpopped_ + output.available_capacity();

  if (first_index <= this->first_unassembled_) {
    std::size_t before_push = output.bytes_pushed();
    std::size_t begin_to_insert = this->first_unassembled_ - first_index;
    if (begin_to_insert < data.length()) {
      output.push(std::string{data, begin_to_insert});
    }
    std::size_t exact_pushed = output.bytes_pushed() - before_push;
    this->first_unassembled_ += exact_pushed;
    this->try_insert(output);
    if (this->last_substring_arrived_
     && this->first_unassembled_ >= this->last_substring_end_)
    {
      output.close();
    }
  } else {
    std::uint64_t first_unacceptable = this->first_unpopped_ + this->capacity_;
    this->buf_.resize(first_unacceptable, std::nullopt);
    for (std::size_t i = first_index;
         i < first_index + data.length() && i < first_unacceptable;
         i += 1)
    {
      this->buf_[i] = data[i - first_index];
    }
  }
}

std::uint64_t
Reassembler::bytes_pending() const
{
  std::uint64_t count = 0;
  for (std::uint64_t i = this->first_unassembled_; i < this->buf_.size(); i += 1) {
    if (std::nullopt != this->buf_[i]) {
      count += 1;
    }
  }
  return count;
}
