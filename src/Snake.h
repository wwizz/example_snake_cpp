#ifndef EXAMPLE_SNAKE_CPP_SNAKE_H
#define EXAMPLE_SNAKE_CPP_SNAKE_H

#include <data/Point.h>
#include <vector>
#include <input/IInputManager.h>
#include <platform/Timer.h>
#include "Settings.h"

enum MoveDirection {
  MoveDirection_Left,
  MoveDirection_Right,
  MoveDirection_Up,
  MoveDirection_Down,
};

enum SnakeBodyPart {
  SnakeBodyPart_None,
  SnakeBodyPart_Head,
  SnakeBodyPart_Tail,
};

class Snake {
 public:
  Snake(Settings settings, TimeManager *timeManager);
  void start();
  void grow_next();
  void move();
  void set_new_direction(MoveDirection direction);
  SnakeBodyPart get_body_part(int x, int y);
  bool is_alive();
  Point get_head();
  void process_input(const InputCommand &command);
  void stop();
  bool update(const InputCommand &command);
  bool check_if_snake_head_is_at(Point point);
 protected:
  Settings _settings;
  std::vector<Point> _body;
  bool _grow;
  MoveDirection _direction;
  Timer _moveTimer;
  TimeManager *_timeManager;


  void move_body(int x, int y);
  bool _isAlive;
  bool is_part_of_body(Point point);
  bool is_wall(Point point);
  void remove_first_element();
  void add_last_element(const Point &newHead);
  Point get_last_element() const;
};

#endif //EXAMPLE_SNAKE_CPP_SNAKE_H
