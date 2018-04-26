#include "Point.h"
Point::Point(int _x, int _y) : _x(_x), _y(_y) {}
void Point::set_x(int x) {
  _x = x;
}
void Point::set_y(int y) {
  _y = y;
}
int Point::get_x() const {
  return _x;
}
int Point::get_y() const {
  return _y;
}
