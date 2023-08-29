#include "PauseLayer.h"
#include "game/ui/GameUIScene.h"
#include "game/GameScene.h"

namespace UI {
PauseLayer::PauseLayer(GameUIScene *scene)
    : MenuLayer(scene, "ui/Pause.xml")
{
    auto container = getContainer();
    auto gameScene = scene->parentGameScene();
    auto &resetAction = gameScene->getResetAction();
    auto &pauseAction = gameScene->getPauseAction();
    auto &resumeAction = gameScene->getResumeAction();

    auto closeAction = [&resumeAction] {
        resumeAction.trigger();
    };

    setCloseWidget(container, closeAction);
    setCloseWidget(container->findChild<Widget*>("label"), closeAction);

    container->findChild<Widget*>("exit")->setEventListener(Event::Click, [=](Widget*, const Event&) {
        gameScene->getResetAction().trigger();
        scene->showLayerInsteadOf(scene->start, scene->game);
    });

    pauseAction.addOnCompletedListener([this] {
        show();
    });

    auto onResetSub = resetAction.addOnTriggerListener([this] {
        blockClose();
    });

    auto onResetCompletedSub = resetAction.addOnCompletedListener([this] {
        unblockClose();
        close();
    });

    auto onResumeSub = resumeAction.addOnTriggerListener([&resumeAction] {
        resumeAction.lock();
    });

    addSubscriptions(onResetSub, onResetCompletedSub, onResumeSub);
    muteSubscriptions();
}

void PauseLayer::onHide() {
    MenuLayer::onHide();

    auto &resumeAction = parentGameUIScene()->parentGameScene()->getResumeAction();

    if (resumeAction.isLocked()) {
        resumeAction.unlock();
    }
}
} // UI
