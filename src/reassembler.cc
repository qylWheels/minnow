#include "reassembler.hh"

#include <cstdint>

#include <optional>
#include <string>
#include <vector>

Reassembler::Reassembler()
  : next_reassemble_index_(0), cache_() { }

void
Reassembler::insert_cache(std::uint64_t first_index, std::string data)
{
  // expand cache
  if (first_index + data.length() > this->cache_.size()) {
    std::uint64_t expand_size = first_index + data.length() - this->cache_.size() + 128;
    this->cache_.insert(this->cache_.end(), expand_size, std::nullopt);
  }

  // copy
  for (std::uint64_t i = 0; i < data.length(); ++i) {
    this->cache_[first_index + i] = std::optional<char>{data[i]};
  }
}

void
Reassembler::try_reassemble_cache(Writer &output)
{
  std::optional<char> current;
  std::uint64_t avail = output.available_capacity();
  std::uint64_t exact = 0;
  std::string push;

  for (std::uint64_t i = 0;
       i < avail && this->next_reassemble_index_ + i < this->cache_.size();
       ++i)
  {
    current = this->cache_[i];
    if (current == std::nullopt)
      break;
    push += current.value();
    exact += 1;
  }

  output.push(push);
  this->next_reassemble_index_ += exact;
}

void
Reassembler::insert(std::uint64_t first_index, std::string data,
                    bool is_last_substring, Writer& output)
{
  // there is no need to write string into output if it has been closed
  if (output.is_closed()) {
    return;
  }

  if (first_index <= this->next_reassemble_index_
      && first_index + data.size() > this->next_reassemble_index_)
  {
    std::uint64_t avail = output.available_capacity();
    std::uint64_t len = first_index + data.size() - this->next_reassemble_index_;
    std::uint64_t exact = avail > len ? len : avail;
    output.push(std::string(data, this->next_reassemble_index_ - first_index, exact));
    this->next_reassemble_index_ += exact;
    this->try_reassemble_cache(output);
  } else if (first_index > this->next_reassemble_index_) {
    this->insert_cache(first_index, data);
  }

  if (is_last_substring)
    output.close();
}

uint64_t
Reassembler::bytes_pending() const
{
  std::uint64_t pending = 0;
  for (uint64_t i = this->next_reassemble_index_; i < this->cache_.size(); i += 1) {
    if (this->cache_[i] != std::nullopt) {
      pending += 1;
    }
  }

  return pending;
}
