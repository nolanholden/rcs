#include "pch.h"

#include "../rcs/rcs.hh"

#include <chrono>
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

// SCHEDULED TASK TESTS

template <const std::uintmax_t N, typename _task_runner> auto
run_n_times(_task_runner&& runner) {
  auto num_trues = decltype(N){};

  auto n_times = [&] {
    if (num_trues < N) {
      ++num_trues;
      return true;
    }
    return false;
  };

  std::forward<_task_runner>(runner).run_tasks(n_times);
}

int go_called = 0;
auto go() { ++go_called; }
SCHEDULED_TASK(task1, 100, go)

TEST(ScheduledTask, calls) {
  void(* const tasks[])() = { go };
  auto runner = rcs::make_task_runner(tasks);

  run_n_times<0>(runner);
  EXPECT_EQ(0, go_called);

  run_n_times<1>(runner);
  EXPECT_EQ(1, go_called);

  run_n_times<2>(runner);
  EXPECT_EQ(3, go_called);
}
