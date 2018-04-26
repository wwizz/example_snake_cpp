//
// Created by A Hubbers on 20/04/2018.
//

#ifndef EXAMPLE_SNAKE_CPP_IMENURENDERER_H
#define EXAMPLE_SNAKE_CPP_IMENURENDERER_H

#include <vector>
#include <Settings.h>
#include <menu/Menu.h>
#include <GameLogic.h>

class IMenuRenderer {
 public:
  virtual void render(GameLogic *gameLogic) = 0;
};

#endif //EXAMPLE_SNAKE_CPP_IMENURENDERER_H
