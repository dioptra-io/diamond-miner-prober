#pragma once

#include <chrono>
#include <cmath>
#include <iostream>

typedef std::chrono::duration<uint64_t, std::ratio<1, 10000>> tenth_ms;

inline uint16_t encode_timestamp(uint64_t timestamp) {
  return timestamp % 65535;
}

inline uint64_t decode_timestamp(uint64_t timestamp, uint16_t remainder) {
  uint64_t quotient = std::ceil(timestamp / 65535.0) - 1;
  uint64_t decoded = quotient * 65535 + remainder;
  return decoded > timestamp ? decoded - 65535 : decoded;
}

inline uint16_t decode_difference(uint64_t timestamp, uint16_t remainder) {
  return timestamp - decode_timestamp(timestamp, remainder);
}

template <typename Duration, typename TimePoint>
inline uint64_t to_timestamp(TimePoint tp) {
  return std::chrono::duration_cast<Duration>(tp.time_since_epoch()).count();
}