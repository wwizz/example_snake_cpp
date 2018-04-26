#ifndef EXAMPLE_SNAKE_CPP_GAMERENDERDERTERMINAL_H
#define EXAMPLE_SNAKE_CPP_GAMERENDERDERTERMINAL_H

#include <vector>
#include <platform/ITerminal.h>
#include <Settings.h>
#include <Snake.h>
#include <GameLogic.h>
#include "render/IGameRenderer.h"
#include "RendererTerminal.h"
class GameRenderderTerminal: public IGameRenderer, public RendererTerminal {
 public:
  explicit GameRenderderTerminal(ITerminal *pTerminal, int width, int height);
  void render(GameLogic *gameData) override;

 protected:
  void draw_line();
  void draw_lines();


  Snake *_snake;
  void draw_head();
  void draw_tail();
  Point _fruit;
  void draw_fruit();
};

#endif //EXAMPLE_SNAKE_CPP_GAMERENDERDERTERMINAL_H
