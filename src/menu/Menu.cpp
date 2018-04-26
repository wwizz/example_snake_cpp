
#include "Menu.h"
Menu::Menu(std::string title, std::vector<std::string> options) :
  _title(std::move(title)),
  _options(std::move(options)){
}

Menu::~Menu() {

}

const std::string &Menu::GetTitle() const {
  return _title;
}

std::vector<std::string> Menu::GetOptions() const {
  return _options;
}
