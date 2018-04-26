#include <platform/posx/TerminalPosx.h>
#include <input/InputManagerTerminal.h>
#include <render/terminal/GameRenderderTerminal.h>
#include <huestream/config/Config.h>
#include <huestream/HueStream.h>
#include "Engine.h"
#include "render/terminal/MenuRenderderTerminal.h"

std::shared_ptr<huestream::HueStream> connect_to_hue();

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

  auto huestream = connect_to_hue();

  Engine(inputManager, frameManager, gameLogic, renderer, huestream).run();

  delete renderer;
  delete gameRenderer;
  delete menuRenderer;
  delete frameManager;
  delete gameLogic;
  delete inputManager;
  delete timeManager;
  delete terminal;
}

std::shared_ptr<huestream::HueStream> connect_to_hue() {
  //connect to in synchrone manner
  auto config = std::make_shared<huestream::Config>("snake", "pc");
  auto huestream = std::make_shared<huestream::HueStream>(config);

  //  Register feedback callback
  huestream->RegisterFeedbackCallback([](const huestream::FeedbackMessage &message) {
    std::cout << "[" << message.GetId() << "] " << message.GetTag() << std::endl;
    if (message.GetId() == huestream::FeedbackMessage::ID_DONE_COMPLETED) {
      std::cout << "bridge-ip: " << message.GetBridge()->GetIpAddress() << std::endl;
      std::cout << "bridge-username: " << message.GetBridge()->GetUser() << std::endl;
      std::cout << "bridge-clientkey: " << message.GetBridge()->GetClientKey() << std::endl;
    }
    if (message.GetMessageType() == huestream::FeedbackMessage::FEEDBACK_TYPE_USER) {
      std::cout << message.GetUserMessage() << std::endl;
    }
  });

  huestream->ConnectBridge();

  if (!huestream->IsBridgeStreaming()) {
    auto bridge = huestream->GetLoadedBridge();

    if (bridge->GetStatus() == huestream::BRIDGE_INVALID_GROUP_SELECTED) {
      std::cout << "Please configure only one group in the hue bridge" << std::endl;
    } else {
      std::cout << "No streamable bridge configured: " << bridge->GetStatusTag() << std::endl;
    }
  }
  return huestream;
}