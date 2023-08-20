#include "PauseLayer.h"

#include <algine/core/widgets/Container.h>

namespace UI {
PauseLayer::PauseLayer(GameUIScene *scene)
    : MenuLayer(scene, "ui/Pause.xml")
{
    setCloseWidget(getContainer());
    setCloseWidget(getContainer()->findChild<Widget*>("label"));
    setCloseWidget(getContainer()->findChild<Widget*>("exit"), [scene]() {
        // TODO
//        auto game = Game::getInstance();
//        game->saveProgress();
//        game->reset();
//        game->getUI().showLayerInsteadOf(scene->start, scene->game);
    });
}
} // UI
