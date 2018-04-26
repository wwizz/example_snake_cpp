#ifndef EXAMPLE_SNAKE_CPP_IGAMERENDERER_H
#define EXAMPLE_SNAKE_CPP_IGAMERENDERER_H

#include <vector>
#include <Settings.h>
#include <Snake.h>
#include <GameLogic.h>

class IGameRenderer {
 public:
  virtual void render(GameLogic *gameData) = 0;
};

#endif //EXAMPLE_SNAKE_CPP_IGAMERENDERER_H
