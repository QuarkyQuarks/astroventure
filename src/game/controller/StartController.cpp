#include "StartController.h"
#include "GameController.h"
#include "game/GameScene.h"

StartController::StartController(GameScene &gameScene)
    : Controller(gameScene) {}

void StartController::update() {}

void StartController::event(const algine::Event &event) {
    if (event.getId() != Event::Id::Click)
        return;

    m_gameScene.getCameraman().startAnimation(Cameraman::Animation::Intro);

    // do not do anything after this line. It will
    // cause the deletion of the current controller.
    m_gameScene.setController(std::make_unique<GameController>(m_gameScene));
}
