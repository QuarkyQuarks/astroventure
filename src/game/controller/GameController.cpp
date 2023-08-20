#include "GameController.h"
#include "game/GameScene.h"

#include <algine/core/log/Log.h>

#define LOG_TAG "GameController"

using namespace algine;

GameController::GameController(GameScene &gameScene)
    : Controller(gameScene)
{
    auto &mechanics = gameScene.getMechanics();

    mechanics.addOnLaunchListener([] {
        Log::debug(LOG_TAG, "onLaunch");
    });

    mechanics.addOnGroundListener([](Planet *planet) {
        Log::debug(LOG_TAG, std::format("onGround, planet index = {}", planet->getIndex()));
    });
}

void GameController::update() {
    m_gameScene.getMechanics().update();
}

void GameController::event(const Event &event) {
    Log::debug(LOG_TAG, __func__);

    if (event.getId() != Event::Id::Click)
        return;

    m_gameScene.getMechanics().launch();
}
