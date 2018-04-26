#ifndef EXAMPLE_SNAKE_CPP_TIME_H
#define EXAMPLE_SNAKE_CPP_TIME_H

#include <cstdint>
#include <chrono>
#include <atomic>
class TimeManager {
 public:
  TimeManager();
  int64_t now();
  void update();
  void sleep(int64_t sleepTime);

 protected:
  std::atomic<int64_t> _now;
  std::chrono::steady_clock::time_point _start;
};

#endif //EXAMPLE_SNAKE_CPP_TIME_H
