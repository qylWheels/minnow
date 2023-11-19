#pragma once

#include "byte_stream.hh"

#include <cstdint>

#include <optional>
#include <string>
#include <vector>

class Reassembler
{
private:
  // Read the picture in p3 in check1.pdf
  std::int64_t first_unpopped_;
  std::int64_t first_unassembled_;
  std::uint64_t capacity_;
  std::vector<std::optional<char>> buf_;

  void
  try_insert(Writer &output);   // try to insert bytes in buffer after inserting successfully

public:
  Reassembler();

  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
  void
  insert(std::uint64_t first_index, std::string data, bool is_last_substring, Writer& output);

  // How many bytes are stored in the Reassembler itself?
  std::uint64_t
  bytes_pending() const;
};
