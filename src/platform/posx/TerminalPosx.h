#ifndef EXAMPLE_SNAKE_CPP_TERMINAL_H
#define EXAMPLE_SNAKE_CPP_TERMINAL_H

#include <platform/ITerminal.h>
class TerminalPosix : public ITerminal {

 public:
  TerminalPosix();
  virtual ~TerminalPosix();
  char get_char() override;
  bool has_input() override;

};

#endif //EXAMPLE_SNAKE_CPP_TERMINAL_H
