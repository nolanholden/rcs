#ifndef RCS_TYPES_HH__
#define RCS_TYPES_HH__

#include <type_traits>
#include <limits>

#ifndef RCS_USE_FAST_INT
# define RCS_USE_FAST_INT 0
#endif

#ifndef RCS_ASSERT_IEC559_FLOATING_POINTS
# define RCS_ASSERT_IEC559_FLOATING_POINTS 1
#endif


#ifndef RCS_BYTE_TYPE
# include <cstdint>
# if RCS_USE_FAST_INT
#   define RCS_BYTE_TYPE std::uint_fast8_t
# else
#   define RCS_BYTE_TYPE std::uint8_t
# endif // RCS_USE_FAST_INT
#endif // !RCS_BYTE_TYPE

#ifndef RCS_FLOAT32_TYPE
# define RCS_FLOAT32_TYPE float
#endif

#ifndef RCS_FLOAT64_TYPE
# define RCS_FLOAT64_TYPE double
#endif

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
using f32_t = RCS_FLOAT32_TYPE;
static_assert(sizeof(f32_t) == 4, "f32_t must be 32 bits (4 bytes) wide");
#if RCS_ASSERT_IEC559_FLOATING_POINTS
static_assert(std::numeric_limits<f32_t>::is_iec559, "must be IEC-559 floating point");
#endif


/**
* @breif System-wide "double" (64-bit floating point) type.
*/
using f64_t = RCS_FLOAT64_TYPE;
static_assert(sizeof(f64_t) == 8, "f64_t must be 64 bits (8 bytes) wide");
#if RCS_ASSERT_IEC559_FLOATING_POINTS
static_assert(std::numeric_limits<f32_t>::is_iec559, "must be IEC-559 floating point");
#endif

} // namespace rcs

#endif RCS_TYPES_HH__
