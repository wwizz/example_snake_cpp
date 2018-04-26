#include <iostream>
#include <menu/Menu.h>
#include <GameLogic.h>
#include "MenuRenderderTerminal.h"

MenuRenderderTerminal::MenuRenderderTerminal(ITerminal *pTerminal, int width, int height):
    RendererTerminal(pTerminal, width, height) {

}

void MenuRenderderTerminal::render(GameLogic *gameLogic) {
  _options = gameLogic->get_menu()->GetOptions();

  clear();
  draw_lines();
}

void MenuRenderderTerminal::draw_lines() {
  for (_line = 0; _line < _height; ++_line) {
    if (isFirstOrLastIndex(_line, _height)) {
      draw_wall_line();
    } else if (_line == 2) {
      draw_options();
      _line += _options.size() - 1;
    }
    else {
      draw_empty_line();
    }
  }
}

void MenuRenderderTerminal::draw_options() {
  for (int i = 0; i < _options.size(); ++i) {
    draw_option(i);
  }
}

void MenuRenderderTerminal::draw_option(int option_index) {
  auto option = _options[option_index];
  auto str = " " + std::to_string(option_index + 1) + " " + option;
  draw_string(str);
}


