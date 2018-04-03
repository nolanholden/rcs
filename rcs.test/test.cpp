#include "gtest/gtest.h"
#include "pch.h"

#include "../rcs/radio.hh"
#include "../rcs/error.hh"
#include "../rcs/utility/serialization.hh"

#include <chrono>
#include <cstddef>
#include <functional>
#include <limits>
#include <ratio>
#include <utility>
#include <vector>

#define ARR_SIZE(arr) (sizeof(arr) / sizeof(*arr))

template <typename T, typename U>
constexpr auto
ceiling_divide(T num, U den) {
  static_assert(std::is_unsigned_v<T>, "only allowed for unsigned");
  static_assert(std::is_unsigned_v<U>, "only allowed for unsigned");
  return num / den + (num % den != 0);
}

using namespace rcs;
using namespace rcs::comm;
using namespace rcs::utility;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%% TELEMETRY TESTS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

TEST(telemetry_send, can_send) {
  byte_t bytes[] = { 0,1,2,3,4,5,6,7,8,9, };
  std::size_t max_allowed = ARR_SIZE(bytes) + 1; // larger than whole buffer
  payload p{ bytes, ARR_SIZE(bytes) };
  auto&& send_bytes_impl = [&](const payload& payload) {
    for (auto i = byte_t{}; i < payload.size; ++i)
      EXPECT_EQ(i, payload.bytes[i]);
    return error::none;
  };
  radio_config c{ std::move(send_bytes_impl), max_allowed };
  send_partitioned_payload(p, c);
}

// Tests (in general) that the auto-partitioning of radio payloads sent to 
// our API(s) with sizes larger than the given max payload size (perhaps 
// imposed by time constraints or device limitations) is correct.
// 
// Specifically, we test that the `bytes` passed in each have a value exactly 
// equal to its index and that the accumulated size passed into our delegated 
// function exactly equals to original payload size.
auto test_partitioning(const payload& _payload, std::size_t max_allowed) {
  std::size_t total_size_received = 0;
  auto buffer_position = std::size_t{};
  auto&& send_bytes_impl = [&](const payload& p) {
    EXPECT_TRUE(p.size > 0); // This delegated function should never be called to
                           // handle zero-size payloads.
    total_size_received += p.size;
    for (auto i = decltype(p.size){}; i < p.size; ++i) {
      EXPECT_EQ(static_cast<byte_t>(buffer_position), p.bytes[i]);
      ++buffer_position;
    }
    return error::none;
  };

  radio_config c{ std::move(send_bytes_impl), max_allowed };
  auto err = send_partitioned_payload(_payload, c);

  // across all partitions, total recvd should be same as size
  EXPECT_EQ(_payload.size, total_size_received);

  return err;
}

TEST(telemetry_send, partitions_overflow_correctly) {
  const std::size_t num_bytes = 513;
  byte_t bytes[num_bytes];
  payload p{ bytes, num_bytes };
  for (auto i = decltype(num_bytes){}; i < num_bytes; ++i)
    bytes[i] = static_cast<byte_t>(i);

  for (auto max_payload_size = 0; ++max_payload_size <= num_bytes;) {
    ASSERT_EQ(error::none, test_partitioning(p, max_payload_size));
  }
}

TEST(telemetry_send, permits_zero_size_payload_without_error) {
  payload p{ nullptr, 0 };
  EXPECT_EQ(error::none,
    test_partitioning(p, 10));
}

// The delegated send function should never be called to
// handle zero-size payloads.
TEST(telemetry_send, delegate_not_called_for_zero_size_payload) {
  payload p{ nullptr, 0 };

  bool called = false;
  auto&& send_bytes_impl = [&](const payload&) {
    called = true;
    return error::none;
  };
  radio_config c{ std::move(send_bytes_impl), 10 };
  EXPECT_EQ(error::none, send_partitioned_payload(p, c));
  EXPECT_FALSE(called);
}

TEST(telemetry_send, complains_about_non_positive_max_payload) {
  payload p{ nullptr, 0 };
  EXPECT_EQ(error::non_positive_max_payload,
    test_partitioning(p, 0));
}

TEST(radio, calls_configured_send_function) {
  bool called = false;
  auto&& send_bytes_impl = [&](const payload&) {
    called = true;
    return error::none;
  };

  radio_config config{ std::move(send_bytes_impl), 2 };
  radio r{ config };
  byte_t bytes[] = { 0,1,2 };
  r.send(payload{ bytes, 3 });
  EXPECT_TRUE(called);
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%% UTILITY TESTS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

template <typename T>
constexpr auto
expect_byte_parsing_result_eq(const T& value_expr) {
  const auto original_value = value_expr;
   
  const auto num_bytes = sizeof(original_value);
  byte_t underlying_bytes[num_bytes];
  append_bytes(underlying_bytes, original_value);
   
  std::decay_t<T> parsed_value;
  from_bytes(parsed_value, underlying_bytes);
  
  if (!(value_expr == value_expr)) { // catch NaN case
    EXPECT_FALSE(original_value == parsed_value);
    EXPECT_FALSE(value_expr == parsed_value);
    EXPECT_FALSE(parsed_value == parsed_value);
  } else {
    EXPECT_EQ(original_value, parsed_value);
    EXPECT_EQ(value_expr, parsed_value);
  }
}

template <typename T>
auto expect_byte_parsing_results_eq_for_type() {
  expect_byte_parsing_result_eq( T{});
  expect_byte_parsing_result_eq(-T{});
  expect_byte_parsing_result_eq( std::numeric_limits<T>::denorm_min());
  expect_byte_parsing_result_eq(-std::numeric_limits<T>::denorm_min());
  expect_byte_parsing_result_eq( std::numeric_limits<T>::epsilon());
  expect_byte_parsing_result_eq(-std::numeric_limits<T>::epsilon());
  expect_byte_parsing_result_eq( std::numeric_limits<T>::infinity());
  expect_byte_parsing_result_eq(-std::numeric_limits<T>::infinity());
  expect_byte_parsing_result_eq( std::numeric_limits<T>::lowest());
  expect_byte_parsing_result_eq(-std::numeric_limits<T>::lowest());
  for (T f =  std::numeric_limits<T>::max(); f >  0.; f /= static_cast<T>(5.1f))
    expect_byte_parsing_result_eq(T{ f });
  for (T f = -std::numeric_limits<T>::max(); f < -0.; f /= static_cast<T>(5.1f))
    expect_byte_parsing_result_eq(T{ f });
  expect_byte_parsing_result_eq( std::numeric_limits<T>::min());
  expect_byte_parsing_result_eq(-std::numeric_limits<T>::min());
  expect_byte_parsing_result_eq( std::numeric_limits<T>::quiet_NaN());
  expect_byte_parsing_result_eq(-std::numeric_limits<T>::quiet_NaN());
  expect_byte_parsing_result_eq( std::numeric_limits<T>::signaling_NaN());
  expect_byte_parsing_result_eq(-std::numeric_limits<T>::signaling_NaN());
  expect_byte_parsing_result_eq( std::numeric_limits<T>::round_error());
  expect_byte_parsing_result_eq(-std::numeric_limits<T>::round_error());
}

TEST(append_bytes, floating_point) {
  using namespace std;
  expect_byte_parsing_results_eq_for_type<int8_t>();
  expect_byte_parsing_results_eq_for_type<int16_t>();
  expect_byte_parsing_results_eq_for_type<int32_t>();
  expect_byte_parsing_results_eq_for_type<int64_t>();
  expect_byte_parsing_results_eq_for_type<uint8_t>();
  expect_byte_parsing_results_eq_for_type<uint16_t>();
  expect_byte_parsing_results_eq_for_type<uint32_t>();
  expect_byte_parsing_results_eq_for_type<uint64_t>();

  expect_byte_parsing_results_eq_for_type<int_least8_t>();
  expect_byte_parsing_results_eq_for_type<int_least16_t>();
  expect_byte_parsing_results_eq_for_type<int_least32_t>();
  expect_byte_parsing_results_eq_for_type<int_least64_t>();
  expect_byte_parsing_results_eq_for_type<uint_least8_t>();
  expect_byte_parsing_results_eq_for_type<uint_least16_t>();
  expect_byte_parsing_results_eq_for_type<uint_least32_t>();
  expect_byte_parsing_results_eq_for_type<uint_least64_t>();

  expect_byte_parsing_results_eq_for_type<int_fast8_t>();
  expect_byte_parsing_results_eq_for_type<int_fast16_t>();
  expect_byte_parsing_results_eq_for_type<int_fast32_t>();
  expect_byte_parsing_results_eq_for_type<int_fast64_t>();
  expect_byte_parsing_results_eq_for_type<uint_fast8_t>();
  expect_byte_parsing_results_eq_for_type<uint_fast16_t>();
  expect_byte_parsing_results_eq_for_type<uint_fast32_t>();
  expect_byte_parsing_results_eq_for_type<uint_fast64_t>();

  expect_byte_parsing_results_eq_for_type<intmax_t>();
  expect_byte_parsing_results_eq_for_type<uintmax_t>();

  expect_byte_parsing_results_eq_for_type<float>();
  expect_byte_parsing_results_eq_for_type<double>();
}
