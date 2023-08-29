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
        scene->parentGameScene()->triggerReset();
        scene->showLayerInsteadOf(scene->start, scene->game);
    };

    auto container = getContainer();
    container->setEventListener(Event::Click, clickAction);
    container->findChild<Widget*>("label")->setEventListener(Event::Click, clickAction);

    auto gameScene = scene->parentGameScene();

    gameScene->getMechanics().addOnDestroyedListener([this] {
        show();
    });

    auto onResetSub = gameScene->addOnResetListener([this] {
        blockClose();
    });

    auto onResetCompletedSub = gameScene->addOnResetCompletedListener([this] {
        unblockClose();
        close();
    });

    addSubscriptions(onResetSub, onResetCompletedSub);
    muteSubscriptions();
}
} // UI
