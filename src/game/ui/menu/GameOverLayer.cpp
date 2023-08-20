#include "GameOverLayer.h"
#include "game/ui/GameUIScene.h"

namespace UI {
GameOverLayer::GameOverLayer(GameUIScene *scene)
    : MenuLayer(scene, "ui/GameOver.xml")
{
    CloseAction closeAction = [scene]() {
//        auto game = Game::getInstance(); TODO
//        game->reset();
        scene->showLayerInsteadOf(scene->start, scene->game);
    };

    setCloseWidget(getContainer(), closeAction);
    setCloseWidget(getContainer()->findChild<Widget*>("label"), closeAction);
}
} // UI
