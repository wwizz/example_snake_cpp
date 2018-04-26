#include <iostream>

#include "Renderer.h"

Renderer::Renderer(IMenuRenderer *menuRenderer, IGameRenderer *gameRenderer)
    : _menuRenderer(menuRenderer),
      _gameRenderer(gameRenderer) {}


void Renderer::render(GameLogic *gameLogic) {
  switch (gameLogic->get_state()) {
    case GameState_Menu:_menuRenderer->render(gameLogic);
      break;
    case GameState_Running:_gameRenderer->render(gameLogic);
      break;
    case GameState_Shutdown:
      std::cout << "\033[2J\033[1;1H";
      std::cout << "Bye bye" << std::endl;
      break;
    case GameState_Init:break;
  }
}
