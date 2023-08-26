#include "GameOverLayer.h"
#include "game/ui/GameUIScene.h"
#include "game/GameScene.h"

namespace UI {
GameOverLayer::GameOverLayer(GameUIScene *scene)
    : MenuLayer(scene, "ui/GameOver.xml")
{
    auto clickAction = [scene, this](Widget*, const Event&) {
        if (isFlagEnabled(MenuLayer::Flag::Close))
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

    gameScene->addOnResetCompletedListener([this] {
        close();
    });
}
} // UI
