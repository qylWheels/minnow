#include <cstdint>

#include <deque>
#include <stdexcept>
#include <string_view>

#include "byte_stream.hh"

ByteStream::ByteStream( uint64_t capacity )
  : capacity_(capacity), buf_(), bytes_pushed_(0),
    bytes_popped_(0), close_(false), error_(false) {}

void
Writer::push( std::string data )
{
  // Your code here.
  const std::uint64_t avail = this->available_capacity();
  const std::uint64_t len = static_cast<uint64_t>(data.size());
  const std::uint64_t copy = avail > len ? len : avail;
  this->bytes_pushed_ += copy;
  for (std::uint64_t i = 0; i < copy; ++i) {
    this->buf_.push_back(data[i]);
  }
}

void
Writer::close()
{
  // Your code here.
  this->close_ = true;
}

void
Writer::set_error()
{
  // Your code here.
  this->error_ = true;
}

bool
Writer::is_closed() const
{
  // Your code here.
  return this->close_;
}

uint64_t
Writer::available_capacity() const
{
  // Your code here.
  return this->capacity_ - static_cast<uint64_t>(this->buf_.size());
}

uint64_t
Writer::bytes_pushed() const
{
  // Your code here.
  return this->bytes_pushed_;
}

std::string_view
Reader::peek() const
{
  // Your code here.
  static std::string peek_result;
  peek_result.clear();
  peek_result.append(this->buf_.begin(), this->buf_.end());
  return std::string_view(peek_result);
}

bool
Reader::is_finished() const
{
  // Your code here.
  return this->close_ && this->buf_.size() == 0;
}

bool
Reader::has_error() const
{
  // Your code here.
  return this->error_;
}

void
Reader::pop( std::uint64_t len )
{
  // Your code here.
  const std::uint64_t size = static_cast<std::uint64_t>(this->buf_.size());
  const std::uint64_t pop = size > len ? len : size;
  this->bytes_popped_ += pop;
  for (std::uint64_t i = 0; i < pop; ++i) {
    this->buf_.pop_front();
  }
}

std::uint64_t
Reader::bytes_buffered() const
{
  // Your code here.
  return this->buf_.size();
}

std::uint64_t
Reader::bytes_popped() const
{
  // Your code here.
  return this->bytes_popped_;
}
