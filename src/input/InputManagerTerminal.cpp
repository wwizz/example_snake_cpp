#include <string>
#include "InputManagerTerminal.h"

InputManagerTerminal::InputManagerTerminal(ITerminal *terminal):_terminal(terminal) {

}

InputCommands InputManagerTerminal::get() {
  auto inputs = InputCommands();
  std::string s = "";
  while(_terminal->has_input()) {
    s += _terminal->get_char();
  }
  for (int i = 0; i < s.size(); ++i) {
    auto ch = s[i];
    if (ch == ASCII_ESCAPE) {
      check_special_escape(&inputs, s, &i);
    } else if (ch == '1') {
      inputs.push_back(InputCommand_Option1);
    } else if (ch == '2') {
      inputs.push_back(InputCommand_Option2);
    } else if (ch == '3') {
      inputs.push_back(InputCommand_Option3);
    } else if (ch == '4') {
      inputs.push_back(InputCommand_Option4);
    } else if (ch == '5') {
      inputs.push_back(InputCommand_Option5);
    } else if (ch == 'w') {
      inputs.push_back(InputCommand_Up);
    } else if (ch == ASCII_UP) {
      inputs.push_back(InputCommand_Up);
    } else if (ch == 'a') {
      inputs.push_back(InputCommand_Left);
    } else if (ch == ASCII_LEFT) {
      inputs.push_back(InputCommand_Left);
    } else if (ch == 's') {
      inputs.push_back(InputCommand_Down);
    } else if (ch == ASCII_DOWN) {
      inputs.push_back(InputCommand_Down);
    } else if (ch == 'd') {
      inputs.push_back(InputCommand_Right);
    } else if (ch == ASCII_RIGHT) {
      inputs.push_back(InputCommand_Right);
    }
  }



  if (inputs.empty()) {
    inputs.push_back(InputCommand_None);
  }
  return inputs;
}
void InputManagerTerminal::check_special_escape(std::vector<InputCommand> *commands,
                                                const std::string &input_str,
                                                int *index) {
  auto l = input_str.size();
  if (*index + 3 <= l) {
    if (input_str[*index + 1] == '[') {
      switch(input_str[*index + 2]) {
        case 'A':
          *index+=2;
          commands->push_back(InputCommand_Up);
          return;
        case 'B':
          *index+=2;
          commands->push_back(InputCommand_Down);
          return;
        case 'C':
          *index+=2;
          commands->push_back(InputCommand_Right);
          return;
        case 'D':
          *index+=2;
          commands->push_back(InputCommand_Left);
          return;
        default:
          break;
      }
    }
  }
  commands->push_back(InputCommand_Escape);

}
