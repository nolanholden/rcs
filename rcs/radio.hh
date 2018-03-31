#ifndef RCS_RADIO_HH__
#define RCS_RADIO_HH__

#include "rcsdef.hh"

#include <cstddef>
#include <functional>
#include <type_traits>

namespace rcs {
namespace comm {
namespace tele {

struct payload { // TODO: use std::array<,> or similar
  // TODO: address?
  const byte_t* bytes;
  std::size_t size;
};
struct radio_config {
  std::function<error(const payload&)> send_payload;
  std::size_t max_payload_size;
};

auto send(const payload& p, const radio_config& c) {
  const auto& max_length = c.max_payload_size;
  if (max_length < 1)
    return error::non_positive_max_payload;
  if (p.size < 1) return error::none;

  auto remaining_size = p.size;
  auto bytes_position = p.bytes;

  while (remaining_size > max_length) {
    RCS_CATCH(err, c.send_payload({ bytes_position, max_length }), return err);
    remaining_size -= max_length;
    bytes_position += max_length;
  }

  return c.send_payload({ bytes_position, remaining_size });
}


class radio {
 public:
  radio_config config{};

  template <class T>
  auto send(T&& payload) {
    return config.send_payload(std::forward<T>(payload));
  }
};

} // namespace tele
} // namespace comm
} // namespace rcs

#endif // RCS_RADIO_HH__
