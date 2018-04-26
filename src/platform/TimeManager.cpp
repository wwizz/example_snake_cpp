#include "TimeManager.h"
#include <thread>


TimeManager::TimeManager() :
    _now(0),
    _start(std::chrono::steady_clock::now()) {
}

int64_t TimeManager::now() {
  return _now;
}

void TimeManager::update() {
  _now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _start).count();
}
void TimeManager::sleep(int64_t sleepTime) {
  std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
}
