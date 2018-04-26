#ifndef EXAMPLE_SNAKE_CPP_FRAMESPEEDMANAGER_H
#define EXAMPLE_SNAKE_CPP_FRAMESPEEDMANAGER_H

#include <platform/TimeManager.h>
class FrameSpeedManager {
 public:
  FrameSpeedManager(TimeManager *pTime, int fps);

  void start();
  void end();
 protected:
  TimeManager* _time;
  int64_t _time_frame;
  int _fps;
};

#endif //EXAMPLE_SNAKE_CPP_FRAMESPEEDMANAGER_H
