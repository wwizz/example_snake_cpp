#include <platform/posx/TerminalPosx.h>
#include <input/InputManagerTerminal.h>
#include <render/terminal/GameRenderderTerminal.h>
#include "Engine.h"
#include "render/terminal/MenuRenderderTerminal.h"
int main() {
  auto settings = Settings { 80, 25};
  auto terminal = new TerminalPosix();
  auto timeManager = new TimeManager();
  auto inputManager = new InputManagerTerminal(terminal);
  auto gameLogic = new GameLogic(timeManager, settings);
  auto frameManager = new FrameSpeedManager(timeManager, 50);
  auto menuRenderer = new MenuRenderderTerminal(terminal, settings.width, settings.height);
  auto gameRenderer = new GameRenderderTerminal(terminal, settings.width, settings.height);
  auto renderer = new Renderer(menuRenderer, gameRenderer);

  Engine(inputManager, frameManager, gameLogic, renderer).run();

  delete renderer;
  delete gameRenderer;
  delete menuRenderer;
  delete frameManager;
  delete gameLogic;
  delete inputManager;
  delete timeManager;
  delete terminal;
}