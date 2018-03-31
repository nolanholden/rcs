//#include "pch.h"
//
//#include "../rcs/rcs.hh"
//
//#include <chrono>
//#include <cstddef>
//#include <functional>
//#include <utility>
//#include <vector>
//
//// SCHEDULED TASK TESTS
//
//template <const std::uintmax_t N, typename _task_runner> auto
//run_n_times(_task_runner&& runner) {
//  auto num_trues = decltype(N){};
//
//  auto n_times = [&] {
//    if (num_trues < N) {
//      ++num_trues;
//      return true;
//    }
//    return false;
//  };
//
//  std::forward<_task_runner>(runner).run_tasks(n_times);
//}
//
//int go_called = 0;
//auto go() { ++go_called; }
//
//TEST(TaskRunner, calls_successfully) {
//  void(*const tasks[])() = { go };
//  auto runner = rcs::make_task_runner(tasks);
//
//  run_n_times<0>(runner);
//  EXPECT_EQ(0, go_called);
//
//  run_n_times<1>(runner);
//  EXPECT_EQ(1, go_called);
//
//  run_n_times<2>(runner);
//  EXPECT_EQ(3, go_called);
//}
//
//SCHEDULED_TASK(task1, 2, go)
//
//class mock_system_clock {
// public:
//  std::uintmax_t current_micros = 0;
//  auto micros() { return current_micros++; }
//};
//
//TEST(ScheduledTask, calls) {
//  mock_system_clock clock{};
//  auto task1_impl = [&] { task1(clock); };
//  //void(* const tasks[])() = { task1_impl };
//  //auto runner = rcs::make_task_runner(tasks);
//}

#include "pch.h"

#include "../rcs/rcs.hh"
#include "../rcs/radio.hh"

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
using namespace rcs::utility;
using namespace rcs::comm;
using namespace rcs::comm::tele;

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
  send(p, c);
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
  auto err = send(_payload, c);

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
  EXPECT_EQ(error::none, send(p, c));
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