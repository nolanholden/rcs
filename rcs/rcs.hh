#ifndef RCS_HH__
#define RCS_HH__

#include <chrono>
#include <type_traits>
#include <utility>


namespace rcs {

template <std::uintmax_t Id>
struct task_info {
  static constexpr const char* description = "default";
};

#define SCHEDULED_TASK( \
    function_name, \
    await_time, \
    action) \
  template <typename T> \
  void function_name(T&& clock) { \
    using us_time_type = typename std::decay<decltype(clock.micros())>::type; \
    static_assert(std::is_unsigned<us_time_type>::value, "system time must be given as unsigned integral value"); \
    static_assert(std::is_integral<decltype(await_time)>::value && await_time > 0, "must await a positive integral time interval"); \
    \
    static us_time_type last_time = 0; \
    const auto& now = std::forward<T>(clock).micros(); \
    const auto& delta = now - last_time;  /* defined by ISO C++ */ \
    last_time = now; \
    if (delta >= await_time) action(); \
  }

auto go() {}
SCHEDULED_TASK(do_every_100, 100, go)

enum class task_id {
  send_telemetry,
  update_imu,
  total_ids
};
struct task {
  bool should_run;
  auto run() const {};
};

template <typename TaskIdEnum>
class task_runner {
 public:
  using id_t = TaskIdEnum;

 private:
  constexpr auto 
  id_index(id_t id) { return static_cast<std::size_t>(id); };

  task* tasks[TaskIdEnum::NUM_TASKS];

  auto enable_impl(id_t id, bool should_run) {
    tasks[id_index(id)] = should_run;
  }

 public:
  task_runner& add_task(task t) { tasks.emplace(std::move(t)); }
  auto enable(id_t id) { enable_impl(id, true); }
  auto disable(id_t id) { enable_impl(id, false); }

  constexpr inline auto 
  run_all() const {
    for (const auto& p : tasks)
      if (p.second != nullptr && p.second.should_run)
        p.second.run();
  }

  // cease when _continue() returns false;
  template <typename ConditionFunc>
  constexpr auto
  run_while(ConditionFunc&& _continue) const {
    auto&& continue__ = std::forward<ConditionFunc>(_continue);
    while (continue__()) run_all();
  }
};

void setup() {
  //using namespace std::literals::chrono_literals;
  //auto s = 1s;
  // initialize digital pin 13 as an output.
  /*pinMode(13, OUTPUT);*/
}

// the loop function runs over and over again forever
void loop() {
  //digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(1000);              // wait for a second
  //digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  //delay(1000);              // wait for a second
}

} // namespace rcs

#endif // RCS_HH__