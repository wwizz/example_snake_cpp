//
// Created by A Hubbers on 20/04/2018.
//

#ifndef EXAMPLE_SNAKE_CPP_MAINMAIN_H
#define EXAMPLE_SNAKE_CPP_MAINMAIN_H

#include "Menu.h"
class MainMenu : public Menu {
 public:
  MainMenu();
  Action Process(InputCommand command) override;
  virtual ~MainMenu();

};

#endif //EXAMPLE_SNAKE_CPP_MAINMAIN_H
