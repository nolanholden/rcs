#ifndef RCS_RCSDEF_HH__
#define RCS_RCSDEF_HH__

#include <cstddef>
#include <type_traits>

namespace rcs {

// Preffered system "byte" type.
using byte_t = std::uint8_t;

/**
 * @breif System-wide error codes
 * @notes Used to supplant C++ exceptions. Exceptions are disabled.
 *
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
 * @sa rcs::utility::typeof
 *
 */
enum class error_category {
  none,
  telecomm,
  uart,
  unknown
};

namespace utility {

auto typeof(error e) {
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
 *    RCS_CATCH(err, do_something_risky(), {
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