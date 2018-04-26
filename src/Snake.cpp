
#include "Snake.h"

Snake::Snake(Settings settings, TimeManager *timeManager) :
    _settings(settings),
    _moveTimer(timeManager){
}

void Snake::start() {
  _isAlive = true;
  _grow = false;
  _body.clear();
  _body.emplace_back(_settings.width / 2, _settings.height / 2);
  _direction = MoveDirection_Right;
  _moveTimer.start(500);

}

void Snake::grow_next() {
  _grow = true;
}

void Snake::move() {
  if (!_isAlive) {
    return;
  }

  switch (_direction) {
    case MoveDirection_Left: return move_body(-1, 0);
    case MoveDirection_Right:return move_body(1, 0);
    case MoveDirection_Up:return move_body(0, -1);
    case MoveDirection_Down:return move_body(0, 1);
  }
}

void Snake::move_body(int x, int y) {
  if (!is_alive()) {
    return;
  }

  auto oldHead = get_last_element();
  auto newHead = Point(oldHead.get_x() + x, oldHead.get_y() + y);

  if (is_part_of_body(newHead) || is_wall(newHead)) {
    _isAlive = false;
    return;
  }

  add_last_element(newHead);
  if (!_grow) {
    remove_first_element();
  }
  _grow = false;
}

Point Snake::get_last_element() const {
  return _body[_body.size() - 1];
}

void Snake::add_last_element(const Point &newHead) {
  _body.push_back(newHead);
}

void Snake::remove_first_element() {
  _body.erase (_body.begin());
}

void Snake::set_new_direction(MoveDirection direction) {
  _direction = direction;
}

SnakeBodyPart Snake::get_body_part(int x, int y) {
  for (int i = 0; i < _body.size(); ++i) {
    auto bodyPoint = _body[i];
    if (bodyPoint.get_x() == x && bodyPoint.get_y() == y) {
      if (i == _body.size() - 1) {
        return SnakeBodyPart_Head;
      }
      return SnakeBodyPart_Tail;
    }
  }
  return SnakeBodyPart_None;
}
bool Snake::is_part_of_body(Point point) {
  return get_body_part(point.get_x(), point.get_y()) != SnakeBodyPart_None;
}

bool Snake::is_wall(Point point) {
  return point.get_x() <= 0 || point.get_x() >= _settings.width - 1 ||
         point.get_y() <= 0 || point.get_y() >= _settings.height - 1;
}
bool Snake::is_alive() {
  return _isAlive;
}

Point Snake::get_head() {
  return get_last_element();
}
void Snake::process_input(const InputCommand &command) {
  switch (command) {
    case InputCommand_None:break;
    case InputCommand_Escape:break;
    case InputCommand_Option1:break;
    case InputCommand_Option2:break;
    case InputCommand_Option3:break;
    case InputCommand_Option4:break;
    case InputCommand_Option5:break;
    case InputCommand_Left:set_new_direction(MoveDirection_Left);
      break;
    case InputCommand_Right:set_new_direction(MoveDirection_Right);
      break;
    case InputCommand_Up:set_new_direction(MoveDirection_Up);
      break;
    case InputCommand_Down:set_new_direction(MoveDirection_Down);
      break;
  }
}

void Snake::stop() {
  _moveTimer.stop();
}

bool Snake::update(const InputCommand &command) {
  process_input(command);
  if (!_moveTimer.elapsed()) {
    return false;
  }
  _moveTimer.start(250);
  move();
  return true;
}

bool Snake::check_if_snake_head_is_at(Point point) {
  auto snakeHead = get_head();
  if (snakeHead.get_x() == point.get_x() && snakeHead.get_y() == point.get_y()) {
    grow_next();
    return true;
  }
  return false;
}


