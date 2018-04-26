#ifndef EXAMPLE_SNAKE_CPP_ITERMINAL_H
#define EXAMPLE_SNAKE_CPP_ITERMINAL_H

class ITerminal {
 public:
  virtual char get_char() = 0;
  virtual bool has_input() = 0;
};

#endif //EXAMPLE_SNAKE_CPP_ITERMINAL_H
