#ifndef EXAMPLE_SNAKE_CPP_MENURENDERDERTERMINAL_H
#define EXAMPLE_SNAKE_CPP_MENURENDERDERTERMINAL_H

#include <vector>
#include <platform/ITerminal.h>
#include <Settings.h>
#include <menu/Menu.h>
#include <GameLogic.h>
#include "render/IMenuRenderer.h"
#include "RendererTerminal.h"
class MenuRenderderTerminal: public IMenuRenderer, public RendererTerminal {
 public:
  MenuRenderderTerminal(ITerminal *pTerminal, int width, int height);
  void render(GameLogic *gameLogic) override;

 protected:
  void draw_options();
  void draw_option(int option_index);
  void draw_lines();
  std::vector<std::string> _options;

};

#endif //EXAMPLE_SNAKE_CPP_MENURENDERDERTERMINAL_H
