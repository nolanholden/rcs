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

#define QUOTE(str) #str
#define SCHEDULED_TASK( \
    function_name, \
    await_time, \
    action) \
  template <typename T> \
  void function_name(T&& time) { \
    using us_time_type = typename std::decay<decltype(time())>::type; \
    static_assert(std::is_unsigned<us_time_type>::value, "system time must be given as unsigned integral value"); \
    static_assert(std::is_integral<decltype(await_time)>::value && await_time > 0, "must await a positive integral time interval"); \
    \
    static us_time_type last_time = 0; \
    const auto& now = time(); \
    const auto& delta = now - last_time;  /* defined by ISO C++ */ \
    last_time = now; \
    if (delta >= await_time) action(); \
  } \
  \
  template <> struct task_info< __COUNTER__ > { \
    static constexpr const char* description = "task " QUOTE(__COUNTER__) ": "; \
  };

auto go() {}
SCHEDULED_TASK(do_every_100, 100, go)

template <typename T>
struct task_runner {
  const T& tasks;

  template <typename U>
  constexpr task_runner(U&& _tasks)
    : tasks(std::forward<U>(_tasks)) {}

  // cease when _continue() returns false;
  template <typename ConditionFunc>
  constexpr auto
  run_tasks(ConditionFunc&& _continue) const {
    auto&& continue__ = std::forward<ConditionFunc>(_continue);
    while (continue__())
      for (const auto& t : tasks)
        t();
  }
};

template <typename T>
constexpr auto
make_task_runner(T&& tasks) {
  return task_runner<T>(std::forward<T>(tasks));
}

void setup() {
  using namespace std::literals::chrono_literals;
  auto s = 1s;
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