#ifndef RCS_TYPES_HH__
#define RCS_TYPES_HH__

#include <type_traits>
#include <limits>

#ifndef RCS_BYTE_TYPE
#  include <cstdint>
#  ifdef RCS_USE_FAST_INT
#    define RCS_BYTE_TYPE std::uint_fast8_t
#  else
#    define RCS_BYTE_TYPE std::uint8_t
#  endif // RCS_USE_FAST_INT
#endif // !RCS_BYTE_TYPE

#ifndef RCS_FLOAT32_TYPE
#  define RCS_FLOAT32_TYPE float
#endif // !RCS_FLOAT32_TYPE

#ifndef RCS_FLOAT64_TYPE
#  define RCS_FLOAT64_TYPE double
#endif // !RCS_FLOAT64_TYPE

namespace rcs {

/**
 * @breif System-wide "byte" type.
 */
using byte_t = RCS_BYTE_TYPE;
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

} // namespace rcs

#endif RCS_TYPES_HH__
