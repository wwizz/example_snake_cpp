
#include "Timer.h"

Timer::Timer(TimeManager *pTime):
    _time(pTime), _running(false) {
}


void Timer::start(uint32_t interval) {
  _interval = interval;
  _start = _time->now();
  _running = true;
}
bool Timer::elapsed() {
  return  _running && (_time->now() > _start + _interval);
}
void Timer::stop() {
  _running = false;
}


