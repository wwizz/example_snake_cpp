#include <input/InputManagerTerminal.h>
#include <platform/FrameSpeedManager.h>
#include <render/Renderer.h>
#include <huestream/HueStream.h>
#include "Engine.h"
#include "GameLogic.h"

Engine::Engine(InputManagerTerminal *pInputManager,
               FrameSpeedManager *pFrameSpeedManager,
               GameLogic *pGameLogic,
               Renderer *pRenderer,
               huestream::HueStreamPtr huestream) :
    _inputManager(pInputManager),
    _frameManager(pFrameSpeedManager),
    _gameLogic(pGameLogic) ,
    _renderer(pRenderer){
}

void Engine::run() {
  _engineAction = {false, false};

  while(!_engineAction.quit) {
    _frameManager->start();
    update();
    render();
    _frameManager->end();
  }
}

void Engine::update() {
  _inputCommands = _inputManager->get();
  _engineAction = _gameLogic->update(_inputCommands);
}

void Engine::render() {
  if (!_engineAction.needsRender) {
    return;
  }
  _engineAction.needsRender = false;
  _renderer->render(_gameLogic);
}



