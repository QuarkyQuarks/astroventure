#include "StartController.h"
#include "game/GameScene.h"

StartController::StartController(GameScene &gameScene)
    : Controller(gameScene) {}

void StartController::update() {}

void StartController::event(const algine::Event &event) {
    if (event.getId() != Event::Id::Click)
        return;
    m_gameScene.getCameraman().startAnimation(Cameraman::Animation::Intro);
}
