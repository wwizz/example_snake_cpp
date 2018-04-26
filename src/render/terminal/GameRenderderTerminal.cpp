#include <iostream>
#include <menu/Menu.h>
#include <Snake.h>
#include <GameLogic.h>
#include "GameRenderderTerminal.h"

GameRenderderTerminal::GameRenderderTerminal(ITerminal *pTerminal, int width, int height):
    _fruit(0,0),
    RendererTerminal(pTerminal, width, height) {
}

void GameRenderderTerminal::render(GameLogic *gameData) {
  _snake = gameData->get_snake();
  _fruit = gameData->get_fruit();
  std::cout << "\033[2J\033[1;1H";
  draw_lines();
}
void GameRenderderTerminal::draw_lines() {
  for (_line = 0; _line < _height; ++_line) {
    if (isFirstOrLastIndex(_line, _height)) {
      draw_wall_line();
    }
    else {
      if (_snake->is_alive()) {
        draw_line();
      }
      else {
        if (_line == 2) {
          auto s = std::string("Game over");
          draw_string(s);
        } else {
          draw_empty_line();
        }
      }
    }
  }
}

void GameRenderderTerminal::draw_line() {
  for (_col = 0; _col < _width; ++_col) {
    if (isFirstOrLastIndex(_col, _width)) {
      draw_wall();
    }
    else {
      auto bodyPart = _snake->get_body_part(_col, _line);
      switch (bodyPart) {
        case SnakeBodyPart_None:
          if (_fruit.get_x() == _col && _fruit.get_y() == _line) {
            draw_fruit();
          } else {
            draw_space();
          }
          break;
        case SnakeBodyPart_Head:
          draw_head();
          break;
        case SnakeBodyPart_Tail:
          draw_tail();
          break;
      }
    }

  }
  draw_endl();
}

void GameRenderderTerminal::draw_head() {
  std::cout << "O";

}
void GameRenderderTerminal::draw_tail() {
  std::cout << "o";
}
void GameRenderderTerminal::draw_fruit() {
  std::cout << "*";

}
