#include "StartLayer.h"
#include "GameUIScene.h"
#include "AnimTools.h"
#include "game/GameScene.h"

#include <algine/core/widgets/Container.h>
#include <algine/core/widgets/Label.h>

namespace UI {
StartLayer::StartLayer(GameUIScene *scene)
    : Layer(scene)
{
    auto container = Widget::constructFromXMLFile<Container*>("ui/GameStartScreen.xml", this);
    setContainer(container);

    container->setEventListener(Event::Click, [scene](Widget*, const Event &event) {
        scene->start->hide();
        scene->game->show();

        auto controller = scene->parentGameScene()->getController();
        controller->event(event);
    });

    auto settings = container->findChild<Widget*>("settings");
    settings->setEventListener(Event::Click, [scene](Widget*, const Event&) {
        scene->settings->show();
    });

    AnimTools::setButtonAnimation(settings);
}

void StartLayer::updateValues() {
    auto &settingsManager = parentGameUIScene()->parentGameScene()->getSettingsManager();
    getContainer()->findChild<Label*>("crystals")->setText(std::to_string(settingsManager.getCrystals()));
    getContainer()->findChild<Label*>("leaderboard")->setText(std::to_string(settingsManager.getHighScore()));
}
} // UI
