#ifndef RCS_ERROR_HH__
#define RCS_ERROR_HH__

namespace rcs {

/**
 * @breif System-wide error codes
 * @notes Used to supplant C++ exceptions, for compatibility with 
 *   exception-disabled contexts.
 * @sa    RCS_CATCH
 */
enum class error {
  none,
  non_positive_max_payload,
};

} // namespace rcs

#endif // RCS_ERROR_HH__
