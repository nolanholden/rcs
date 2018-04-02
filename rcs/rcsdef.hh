#ifndef RCS_RCSDEF_HH__
#define RCS_RCSDEF_HH__

#include <cstddef>
#include <cstring>
#include <type_traits>
#include <limits>

namespace rcs {

/**
 * @breif System-wide "byte" type.
 */
using byte_t = std::uint8_t;
static_assert(sizeof(byte_t) == 1, "byte_t must be 8 bits (1 byte) wide and unsigned");
static_assert(std::is_unsigned<byte_t>::value, "rcs assumes byte_t is unsigned");

/**
* @breif System-wide "float" (32-bit floating point) type.
*/
using f32_t = float;
static_assert(sizeof(f32_t) == 4, "f32_t must be 32 bits (4 destination) wide");
static_assert(std::numeric_limits<f32_t>::is_iec559, "must be IEEE 754 floating point");

/**
* @breif System-wide "double" (64-bit floating point) type.
*/
using f64_t = double;
static_assert(sizeof(f64_t) == 8, "f64_t must be 64 bits (8 destination) wide");
static_assert(std::numeric_limits<f32_t>::is_iec559, "must be IEEE 754 floating point");

/**
 * @breif System-wide error codes
 * @notes Used to supplant C++ exceptions. Exceptions are disabled.
 */
enum class error {
  none,
  non_positive_max_payload,
};

/**
 * @breif Catch a non-`error::none` `error` error code into a variable named 
 *   `err_var_name`, assigned from `err_returning_expr` with the handling scope 
 *   `on_fail_block`.
 *
\verbatim

```c++
 Usage:
 auto go() {
   // ...
   RCS_CATCH(err, something_risky(), {
     if (err == error::whatever)
       handle_whatever();
   }
   // `err` is now out of scope
 }
 ```

\endverbatim
 */
#define RCS_CATCH(err_var_name, err_returning_expr, on_fail_block) \
  do { \
    error err_var_name = err_returning_expr; \
    if (err_var_name != error::none) \
      on_fail_block; \
  } while (0)

namespace utility {

/**
 * @breif serialize `data`'s bytes into the `destination` byte buffer.
 * @notes `destination` is assumed to be at least valid/allocated for 
 *   `sizeof(data)` bytes. If `Data` has any pointer or reference members, 
 *   their state on a remote machine will (barring a quantum event) be invalid 
 *   (excepting null pointers).
 */
template <typename Byte,
  typename Data> constexpr inline auto
append_bytes(Byte* destination, const Data& data) {
  static_assert(sizeof(Byte) == 1, "Byte must be exactly 8 bits");
  static_assert(std::is_trivial<Data>::value, "template parameter `Data` must be trivial type");
  static_assert(std::is_standard_layout<Data>::value, "template parameter `Data` must be standard layout");
  memcpy(destination, &data, sizeof(data));
}

/**
 * @breif deserialize the `source` buffer bytes into `destination`.
 * @notes `source` is assumed to be at least valid/allocated for `sizeof(destination)` 
 *   bytes, and the bytes are assumed to be a valid state for `destination`.
 *   If `Data` has any pointer or reference members, their state on a remote 
 *   machine will (barring a quantum event) be invalid (excepting null pointers).
 */
template <typename Byte,
  typename Data> constexpr inline auto
from_bytes(Data& destination, const Byte* source) {
  static_assert(sizeof(Byte) == 1, "Byte must be exactly 8 bits");
  static_assert(std::is_trivial<Data>::value, "template parameter `Data` must be trivial type");
  static_assert(std::is_standard_layout<Data>::value, "template parameter `Data` must be standard layout");
  memcpy(&destination, source, sizeof(destination));
}

} // namespace utility

} // namespace rcs

#endif // RCS_RCSDEF_HH__
