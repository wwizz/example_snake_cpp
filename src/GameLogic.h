#ifndef EXAMPLE_SNAKE_CPP_GAMEDATA_H
#define EXAMPLE_SNAKE_CPP_GAMEDATA_H

#include <platform/TimeManager.h>
#include <platform/Random.h>
#include <menu/MainMenu.h>
#include "Settings.h"
#include "Snake.h"
#include "EngineAction.h"

enum GameState {
  GameState_Init,
  GameState_Menu,
  GameState_Running,
  GameState_Shutdown
};

class GameLogic {
 public:
  GameLogic(TimeManager* timeManager, Settings settings);
  Snake* get_snake();
  const Point &get_fruit() const;
  int get_score() const;
  void start_game();
  void stop_game();
  EngineAction update(const InputCommands &inputCommands);
  Menu *get_menu();

 protected:
  Settings _settings;
  Snake _snake;
  Point _fruit;
  int _score;
  Random _random;
  GameState _state;
 public:
  GameState get_state() const;
 protected:
  MainMenu _menu;

  void position_fruit();
  void update(const InputCommand &command);
  void update_state_game(const InputCommand &command);
  void update_state_menu(const InputCommand &command);
  EngineAction _action;
  void update_state_init();
  void update_state_shutdown();
};

#endif //EXAMPLE_SNAKE_CPP_GAMEDATA_H
