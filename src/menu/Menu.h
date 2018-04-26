
#ifndef EXAMPLE_SNAKE_CPP_MENU_H
#define EXAMPLE_SNAKE_CPP_MENU_H

#include <vector>
#include <string>
#include <input/IInputManager.h>
#include "Action.h"
class Menu {
 public:
  Menu(std::string title, std::vector<std::string> options);
  const std::string& GetTitle() const;
  std::vector<std::string> GetOptions() const;
  virtual Action Process(InputCommand command) = 0;
  virtual ~Menu();
 protected:
  std::string _title;
  std::vector<std::string> _options;
};

#endif //EXAMPLE_SNAKE_CPP_MENU_H
