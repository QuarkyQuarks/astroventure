#include "GameOverLayer.h"
#include "game/ui/GameUIScene.h"
#include "game/GameScene.h"

namespace UI {
GameOverLayer::GameOverLayer(GameUIScene *scene)
    : MenuLayer(scene, "ui/GameOver.xml")
{
    auto clickAction = [scene, this](Widget*, const Event&) {
        if (!canBeClosed())
            return;
        scene->parentGameScene()->getResetAction().trigger();
        scene->showLayerInsteadOf(scene->start, scene->game);
    };

    auto container = getContainer();
    container->setEventListener(Event::Click, clickAction);
    container->findChild<Widget*>("label")->setEventListener(Event::Click, clickAction);

    auto gameScene = scene->parentGameScene();
    auto &resetAction = gameScene->getResetAction();

    gameScene->getMechanics().addOnDestroyedListener([this] {
        show();
    });

    auto onResetSub = resetAction.addOnTriggerListener([this] {
        blockClose();
    });

    auto onResetCompletedSub = resetAction.addOnCompletedListener([this] {
        unblockClose();
        close();
    });

    addSubscriptions(onResetSub, onResetCompletedSub);
    muteSubscriptions();
}
} // UI
