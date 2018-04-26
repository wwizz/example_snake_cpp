
#ifndef EXAMPLE_SNAKE_CPP_SNAKEGAME_H
#define EXAMPLE_SNAKE_CPP_SNAKEGAME_H
#include <cstdint>
#include <vector>
#include <render/IMenuRenderer.h>
#include <menu/Menu.h>
#include <render/IGameRenderer.h>
#include <platform/Random.h>
#include <render/Renderer.h>
#include "input/IInputManager.h"
#include "platform/ITerminal.h"
#include "platform/TimeManager.h"
#include "platform/FrameSpeedManager.h"
#include "platform/Timer.h"
#include "Settings.h"
#include "Snake.h"
#include "GameLogic.h"
#include "EngineAction.h"

class Engine {
 public:
  Engine(IInputManager *pInputManager,
         FrameSpeedManager *pFrameSpeedManager,
         GameLogic *pGameLogic,
         Renderer *pRenderer);
  void run();

 protected:
  void render();
  void update();

  IInputManager* _inputManager;
  FrameSpeedManager* _frameManager;
  InputCommands _inputCommands;
  EngineAction _engineAction;
  GameLogic* _gameLogic;
  Renderer* _renderer;
};

#endif //EXAMPLE_SNAKE_CPP_SNAKEGAME_H
