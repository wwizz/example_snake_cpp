#ifndef EXAMPLE_SNAKE_CPP_INPUTMANAGERTERMINAL_H
#define EXAMPLE_SNAKE_CPP_INPUTMANAGERTERMINAL_H

#include <platform/ITerminal.h>
#include "IInputManager.h"
const char ASCII_UP = 72;
const char ASCII_DOWN = 80;
const char ASCII_LEFT = 75;
const char ASCII_RIGHT = 77;
const char ASCII_ESCAPE = 27;
class InputManagerTerminal : public IInputManager {
 public:
  explicit InputManagerTerminal(ITerminal* terminal);
  InputCommands get() override;
 protected:
  ITerminal* _terminal;
  void check_special_escape(std::vector<InputCommand> *commands, const std::string &input_str, int *index);
};

#endif //EXAMPLE_SNAKE_CPP_INPUTMANAGERTERMINAL_H
