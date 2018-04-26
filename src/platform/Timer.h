#ifndef EXAMPLE_SNAKE_CPP_TIMER_H
#define EXAMPLE_SNAKE_CPP_TIMER_H

#include <cstdint>
#include "TimeManager.h"
class Timer {
 public:
  Timer(TimeManager *pTime);
  void start(uint32_t interval);
  bool elapsed();
  void stop();
 protected:
  uint32_t _interval;
  TimeManager* _time;
  int64_t _start;
  bool _running;
};

#endif //EXAMPLE_SNAKE_CPP_TIMER_H
