#ifndef RCS_SERIALIZATION_HH__
#define RCS_SERIALIZATION_HH__

#include <cstring>

namespace rcs {
namespace utility {

/**
 * @breif serialize `data`'s bytes into the `destination` byte buffer.
 * @notes `destination` is assumed to be at least valid/allocated for 
 *   `sizeof(data)` bytes. If `Data` has any pointer or reference members, 
 *   their state on a remote machine will (barring a quantum event) be invalid 
 *   (excepting null pointers).
 */
template <typename Byte, typename Data> constexpr inline auto
append_bytes(Byte* destination, const Data& data) {
  static_assert(sizeof(Byte) == 1, "Byte must be exactly 8 bits");
  static_assert(std::is_trivial<Data>::value, "template parameter `Data` must be trivial type");
  static_assert(std::is_standard_layout<Data>::value, "template parameter `Data` must be standard layout");
  std::memcpy(destination, &data, sizeof(data));
}

/**
 * @breif deserialize the `source` buffer bytes into `destination`.
 * @notes `source` is assumed to be at least valid/allocated for `sizeof(destination)` 
 *   bytes, and the bytes are assumed to be a valid state for `destination`.
 *   If `Data` has any pointer or reference members, their state on a remote 
 *   machine will (barring a quantum event) be invalid (excepting null pointers).
 */
template <typename Byte, typename Data> constexpr inline auto
from_bytes(Data& destination, const Byte* source) {
  static_assert(sizeof(Byte) == 1, "Byte must be exactly 8 bits");
  static_assert(std::is_trivial<Data>::value, "template parameter `Data` must be trivial type");
  static_assert(std::is_standard_layout<Data>::value, "template parameter `Data` must be standard layout");
  std::memcpy(&destination, source, sizeof(destination));
}

} // namespace utility
} // namespace rcs

#endif // RCS_SERIALIZATION_HH__
