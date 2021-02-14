#pragma once

#include <arpa/inet.h>

#include <limits>
#include <string>

/// Common numerical functions with exceptions on invalid inputs.
namespace dminer::Checked {

/// Cast a numeric value to another numeric type and raise if the value doesn't
/// fit.
/// If the value type is included in the destination type, then this is
/// equivalent to a static_cast.
/// @tparam Type the destination type.
/// @tparam Value the source type.
/// @param value the value to cast.
/// @return the value casted to the destination type.
template <typename Type, typename Value>
[[nodiscard]] inline constexpr Type numeric_cast(const Value value) {
  // Compile-time fast-path if Value is included into Type.
  if constexpr ((std::numeric_limits<Value>::min() >=
                 std::numeric_limits<Type>::min()) &&
                (std::numeric_limits<Value>::max() <=
                 std::numeric_limits<Type>::max())) {
    return static_cast<Type>(value);
  }
  // Dynamic check otherwise.
  if ((value >= std::numeric_limits<Type>::min()) &&
      (value <= std::numeric_limits<Type>::max())) {
    return static_cast<Type>(value);
  }
  throw std::invalid_argument{
      "Value (" + std::to_string(value) + ") must be between " +
      std::to_string(std::numeric_limits<Type>::min()) + " and " +
      std::to_string(std::numeric_limits<Type>::max())};
}

/// Cast value and convert endianness from host order to network order.
/// This raises if the value doesn't fit.
/// Use this when the value is from a larger type than the destination type.
/// @tparam To the destination type.
/// @tparam From the source type.
/// @param value the value to cast and convert to network order.
/// @return the value casted to the destination type in network order.
template <typename To, typename From>
[[nodiscard]] inline constexpr To hton(const From value) {
  if constexpr (std::is_same<To, uint16_t>::value) {
    return htons(numeric_cast<uint16_t>(value));
  } else if constexpr (std::is_same<To, uint32_t>::value) {
    return htonl(numeric_cast<uint32_t>(value));
  }
}

/// Equivalent of std::stoul for uint16_t.
/// @param str the string to parse.
/// @return the parsed string.
[[nodiscard]] inline uint16_t stou16(const std::string& str) {
  return numeric_cast<uint16_t>(std::stoul(str));
}

/// Equivalent of std::stoul for uint8_t.
/// @param str the string to parse.
/// @return the parsed string.
[[nodiscard]] inline uint8_t stou8(const std::string& str) {
  return numeric_cast<uint8_t>(std::stoul(str));
}

}  // namespace dminer::Checked
