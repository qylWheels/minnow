#include "reassembler.hh"

#include <cstdint>

#include <optional>
#include <string>
#include <vector>

Reassembler::Reassembler()
  : first_unpopped_(-1), first_unassembled_(-1), capacity_(0), buf_() {}

void
Reassembler::try_insert(Writer &output)
{
  if (std::nullopt == this->buf_[this->first_unassembled_]) {
    return;
  }

  for (std::size_t i = this->first_unassembled_; this->buf_[i] != std::nullopt; i += 1)
  {
    output.push(std::string{1, this->buf_[i].value()});
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

  this->first_unpopped_ = output.reader().bytes_popped();

  if (static_cast<std::int64_t>(first_index) <= this->first_unassembled_) {
    std::size_t before_push = output.bytes_pushed();
    output.push(std::string{data, this->first_unassembled_ - first_index});
    std::size_t exact_pushed = output.bytes_pushed() - before_push;
    if (is_last_substring) {
      output.close();
    }
    this->first_unassembled_ += exact_pushed;
    this->capacity_ = this->first_unassembled_ - this->first_unpopped_ + output.available_capacity();
    this->try_insert(output);
  } else {
    std::uint64_t first_unacceptable = this->first_unpopped_ + this->capacity_;
    this->buf_.resize(first_unacceptable, std::nullopt);
    for (std::size_t i = first_index; i < data.length() && i < first_unacceptable; i += 1) {
      this->buf_[i] = data[i - first_index];
    }
  }
}

std::uint64_t
Reassembler::bytes_pending() const
{
  std::uint64_t count = 0;
  for (std::size_t i = this->first_unassembled_; i < this->buf_.size(); i += 1) {
    if (std::nullopt != this->buf_[i]) {
      count += 1;
    }
  }
  return count;
}
