#include "GameLogic.h"

GameLogic::GameLogic(TimeManager* timeManager, Settings settings):
_settings(settings),
_snake(settings, timeManager),
_fruit(0 ,0),
_score(0),
_random(),
_menu(),
_state(GameState_Init){
}

Snake* GameLogic::get_snake() {
  return &_snake;
}

const Point &GameLogic::get_fruit() const {
  return _fruit;
}

int GameLogic::get_score() const {
  return _score;
}

Menu* GameLogic::get_menu() {
  return &_menu;
}


EngineAction GameLogic::update(const InputCommands &inputCommands) {
  _action = EngineAction{false, false};
  for (auto input : inputCommands) {
    update(input);
  }
  return _action;
}

void GameLogic::update(const InputCommand &command) {
  switch (_state) {
    case GameState_Init:update_state_init();
      break;
    case GameState_Menu:update_state_menu(command);
      break;
    case GameState_Running:update_state_game(command);
      break;
    case GameState_Shutdown:update_state_shutdown();
      break;
  }
}

void GameLogic::update_state_menu(const InputCommand &command) {
  auto action = _menu.Process(command);
  switch (action) {
    case Action_None:
      break;
    case Action_Quit:
      _state = GameState_Shutdown;
      _action.needsRender = true;
      break;
    case Action_Play:
      _state = GameState_Running;
      start_game();
      _action.needsRender = true;
      break;
  }
}

void GameLogic::start_game() {
  _snake.start();
  _score = 0;
  position_fruit();
}
void GameLogic::stop_game() {
  _snake.stop();
}

void GameLogic::update_state_game(const InputCommand &command) {
  if (command == InputCommand_Escape) {
    stop_game();
    _state = GameState_Menu;
    _action.needsRender = true;
    return;
  }

  _action.needsRender = _snake.update(command);

  if (_snake.check_if_snake_head_is_at(_fruit)) {
    position_fruit();
    _score++;
    _action.needsRender = true;
  }
}

void GameLogic::position_fruit() {
  _fruit = Point(_random.get(1, _settings.width - 2), _random.get(1, _settings.height - 2));
}
GameState GameLogic::get_state() const {
  return _state;
}
void GameLogic::update_state_init() {
  _state = GameState_Menu;
  _action.needsRender = true;
}

void GameLogic::update_state_shutdown() {
  _action.needsRender = true;
  _action.quit = true;
}
