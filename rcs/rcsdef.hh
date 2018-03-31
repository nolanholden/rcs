#ifndef RCS_RCSDEF_HH__
#define RCS_RCSDEF_HH__

#include <cstddef>
#include <cstring>
#include <type_traits>
#include <limits>
#include <bitset>

namespace rcs {

/**
 * @breif Preferred system-wide "byte" type.
 * 
 */
using byte_t = std::uint8_t;
static_assert(sizeof(byte_t) == 1, "byte_t must be 8 bits (1 byte) wide and unsigned");
static_assert(std::is_unsigned<byte_t>::value, "rcs assumes byte_t is unsigned");

using f32_t = float;
using f64_t = double;
static_assert(sizeof(f32_t) == 4, "f32_t must be 32 bits (4 destination) wide");
static_assert(sizeof(f64_t) == 8, "f64_t must be 64 bits (8 destination) wide");
static_assert(std::numeric_limits<f32_t>::is_iec559, "must be IEEE 754 floating point");
static_assert(std::numeric_limits<f32_t>::is_iec559, "must be IEEE 754 floating point");

/**
 * @breif System-wide error codes
 * @notes Used to supplant C++ exceptions. Exceptions are disabled.
 */
enum class error {
  none,
  // telecomm:
  non_positive_max_payload,
  failed,
  LAST_TELECOMM,
  // uart:
  uart_not_initialized,
  LAST_UART,
};

/**
 * @breif Categories of rcs::error.
 * @sa rcs::utility::categoryof
 */
enum class error_category {
  none,
  telecomm,
  uart,
  unknown
};

namespace utility {

template <typename Byte,
  typename Data> constexpr inline auto
append_bytes(Byte* destination, const Data& data) {
  static_assert(sizeof(Byte) == 1, "Byte must be exactly 8 bits");
  const auto& data_ptr = &data;
  memcpy(destination, data_ptr, sizeof(data));
}

template <typename Byte,
  typename Data> constexpr inline auto
from_bytes(Data& data, Byte* destination) {
  static_assert(sizeof(Byte) == 1, "Byte must be exactly 8 bits");
  const auto& data_ptr = &data;
  memcpy(data_ptr, destination, sizeof(data));
}

/**
 * @breif Get `e`'s corresponding `error_category`.
 */
constexpr error_category
categoryof(rcs::error e) {
  using int_t = ::std::underlying_type_t<decltype(e)>;
  using cat = error_category;

  const auto i = static_cast<int_t>(e);

  if (i == 0) return error_category::none;
  if (i < static_cast<int_t>(error::LAST_TELECOMM))
    return cat::telecomm;
  if (i < static_cast<int_t>(error::LAST_UART))
    return cat::uart;

  return cat::unknown;
}

} // namespace utility


/*
 *  Catch a non-`error::none` `error` error code into a variable named `err_var_name`,
 *  assigned from `err_returning_expr` with the handling scope `on_fail_block`.
 *
 *  Usage:
 *
 *  auto go() {
 *    // ...
 *    RCS_CATCH(err, something_risky(), {
 *      if (err == error::whatever)
 *        handle_whatever();
 *    }
 *    // `err` is now out of scope
 *  }
 *
 *
 */
#define RCS_CATCH(err_var_name, err_returning_expr, on_fail_block) \
  do { \
    error err_var_name = err_returning_expr; \
    if (err_var_name != error::none) \
      on_fail_block; \
  } while (0)

} // namespace rcs

#endif // RCS_RCSDEF_HH__
