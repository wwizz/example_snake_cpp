#ifndef EXAMPLE_SNAKE_CPP_RENDERERTERMINAL_H
#define EXAMPLE_SNAKE_CPP_RENDERERTERMINAL_H

#include <menu/Menu.h>
#include "render/IMenuRenderer.h"
class RendererTerminal {
 public:
  RendererTerminal(ITerminal* terminal, int width, int height);
 protected:
  void clear();
  bool isFirstOrLastIndex(int i, int max) const;
  void draw_space() const;
  void draw_wall() const;
  ITerminal* _terminal;
  int _width;
  int _height;
  int _line;
  int _col;


  void draw_endl() const;
  void draw_wall_line();
  void draw_empty_line();
  void draw_string(const std::string &str);
};

#endif //EXAMPLE_SNAKE_CPP_RENDERERTERMINAL_H
