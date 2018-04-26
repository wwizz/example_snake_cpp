//
// Created by A Hubbers on 20/04/2018.
//

#ifndef EXAMPLE_SNAKE_CPP_IINPUTMANAGER_H
#define EXAMPLE_SNAKE_CPP_IINPUTMANAGER_H

#include <vector>

enum InputCommand {
  InputCommand_None,
  InputCommand_Escape,
  InputCommand_Option1,
  InputCommand_Option2,
  InputCommand_Option3,
  InputCommand_Option4,
  InputCommand_Option5,
  InputCommand_Left,
  InputCommand_Right,
  InputCommand_Up,
  InputCommand_Down,
};

typedef std::vector<InputCommand> InputCommands;

class IInputManager {
 public:
  IInputManager() = default;
  virtual ~IInputManager()  = default;
  virtual InputCommands get() = 0;

};

#endif //EXAMPLE_SNAKE_CPP_IINPUTMANAGER_H
