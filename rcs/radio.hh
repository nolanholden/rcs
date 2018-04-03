#ifndef RCS_RADIO_HH__
#define RCS_RADIO_HH__

#include "error.hh"
#include "types.hh"
#include "utility/catch.hh"

#include <functional>


namespace rcs {
namespace comm {

struct payload {
  const byte_t* bytes;
  std::size_t size;
};

struct radio_config {
  std::function<error(const payload&)> send_payload;
  std::size_t max_payload_size;
};

class radio {
 public:
  radio_config config{};

  template <class Payload>
  auto send(Payload&& payload) {
    return config.send_payload(std::forward<Payload>(payload));
  }
};

error send_partitioned_payload(const payload& p, const radio_config& c) {
  const auto& max_length = c.max_payload_size;
  if (max_length < 1)
    return error::non_positive_max_payload;
  if (p.size < 1) return error::none; // ok, nothing to send

  auto remaining_size = p.size;
  auto bytes_position = p.bytes;
  while (remaining_size > max_length) {
    RCS_CATCH(err, c.send_payload(payload{ bytes_position, max_length }), return err);
    remaining_size -= max_length;
    bytes_position += max_length;
  }

  return c.send_payload(payload{ bytes_position, remaining_size });
}

} // namespace comm
} // namespace rcs

#endif // RCS_RADIO_HH__
