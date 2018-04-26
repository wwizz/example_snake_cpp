//
// Created by A Hubbers on 23/04/2018.
//

#ifndef EXAMPLE_SNAKE_CPP_RENDERER_H
#define EXAMPLE_SNAKE_CPP_RENDERER_H

#include "IMenuRenderer.h"
#include "IGameRenderer.h"

class Renderer {
 protected:
  IMenuRenderer *_menuRenderer;
 public:
  Renderer(IMenuRenderer *menuRenderer, IGameRenderer *gameRenderer);
 protected:
  IGameRenderer *_gameRenderer;
 public:
  void render(GameLogic *gameLogic);
};

#endif //EXAMPLE_SNAKE_CPP_RENDERER_H
