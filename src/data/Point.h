
#ifndef EXAMPLE_SNAKE_CPP_POINT_H
#define EXAMPLE_SNAKE_CPP_POINT_H

class Point {
 public:
  Point(int x, int y);
  void set_x(int x);
  void set_y(int y);
  int get_x() const;
  int get_y() const;
 protected:
  int _x;
  int _y;
};

#endif //EXAMPLE_SNAKE_CPP_POINT_H
