//
// Created by A Hubbers on 21/04/2018.
//

#include <menu/Menu.h>
#include <iostream>
#include "MenuRenderderTerminal.h"
#include "RendererTerminal.h"
bool RendererTerminal::isFirstOrLastIndex(int i, int max) const {
  return i == 0 || i == max - 1;
}
void RendererTerminal::draw_space() const {
  std::cout << " ";
}
void RendererTerminal::draw_wall() const {
  std::cout << '#';
}
RendererTerminal::RendererTerminal(ITerminal* terminal, int width, int height):
_terminal(terminal),
_width(width),
_height(height){

}
void RendererTerminal::clear() {
  std::cout << "\033[2J\033[1;1H";

}
void RendererTerminal::draw_endl() const {
  std::cout << std::endl;
}
void RendererTerminal::draw_wall_line() {
  for (int i = 0; i < _width; ++i) {
    draw_wall();
  }
  draw_endl();
}
void RendererTerminal::draw_empty_line() {
  for (int i = 0; i < _width; ++i) {
    if (isFirstOrLastIndex(i, _width)) {
      draw_wall();
    }
    else {
      draw_space();
    }
  }
  draw_endl();
}
void RendererTerminal::draw_string(const std::string &str) {
  for (_col = 0; _col < _width; ++_col) {
    if (isFirstOrLastIndex(_col, _width)) {
      draw_wall();
    } else if (_col == 2) {
      std::cout << str;
      _col += str.size() - 1;
    }
    else {
      draw_space();
    }
  }
  draw_endl();
}