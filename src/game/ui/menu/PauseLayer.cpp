#include "PauseLayer.h"
#include "game/ui/GameUIScene.h"
#include "game/GameScene.h"

namespace UI {
PauseLayer::PauseLayer(GameUIScene *scene)
    : MenuLayer(scene, "ui/Pause.xml")
{
    auto container = getContainer();
    auto gameScene = scene->parentGameScene();

    setCloseWidget(container);
    setCloseWidget(container->findChild<Widget*>("label"));

    container->findChild<Widget*>("exit")->setEventListener(Event::Click, [=](Widget*, const Event&) {
        gameScene->triggerReset();
        scene->showLayerInsteadOf(scene->start, scene->game);
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
