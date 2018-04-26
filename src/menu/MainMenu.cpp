#include "Action.h"
#include "MainMenu.h"

MainMenu::MainMenu() :
    Menu(std::string("MainMenu"), std::vector<std::string> { "Start Game", "Quit"}) {
}

MainMenu::~MainMenu() {

}

Action MainMenu::Process(InputCommand command) {
  switch(command) {
    case InputCommand_Escape: break;
    case InputCommand_Option1:return Action_Play;
    case InputCommand_Option2:return Action_Quit;
    case InputCommand_Option3:break;
    case InputCommand_Option4:break;
    case InputCommand_Option5:break;
    case InputCommand_None:break;
    case InputCommand_Left:break;
    case InputCommand_Right:break;
    case InputCommand_Up:break;
    case InputCommand_Down:break;
  }
  return Action_None;
}
