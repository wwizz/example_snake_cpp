
#include "FrameSpeedManager.h"
FrameSpeedManager::FrameSpeedManager(TimeManager *pTime, int fps):
    _time(pTime), _fps(fps) {

}
void FrameSpeedManager::start() {
  _time->update();
  _time_frame = _time->now();
}
void FrameSpeedManager::end() {
  _time->update();
  auto now = _time->now();

  auto renderDuration = now - _time_frame;
  _fps = 20;
  auto _renderTime = (int64_t) ((1.0 / _fps) * 1000);
  auto sleepTime = _renderTime - renderDuration;
  _time->sleep(sleepTime);
}
