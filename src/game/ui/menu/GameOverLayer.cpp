#include "GameOverLayer.h"
#include "game/ui/GameUIScene.h"
#include "game/GameScene.h"

namespace UI {
GameOverLayer::GameOverLayer(GameUIScene *scene)
    : MenuLayer(scene, "ui/GameOver.xml")
{
    CloseAction closeAction = [scene]() {
        scene->parentGameScene()->triggerReset();
        scene->showLayerInsteadOf(scene->start, scene->game);
    };

    setCloseWidget(getContainer(), closeAction);
    setCloseWidget(getContainer()->findChild<Widget*>("label"), closeAction);

    auto gameScene = scene->parentGameScene();

    gameScene->getMechanics().addOnDestroyedListener([this] {
        show();
    });

    gameScene->addOnResetCompletedListener([this] {
        // TODO
    });
}
} // UI
