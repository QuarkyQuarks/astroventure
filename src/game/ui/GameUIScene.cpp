#include "GameUIScene.h"
#include "game/GameScene.h"
#include "game/GameContent.h"

#include <algine/core/assert_cast.h>

namespace UI {
GameUIScene::GameUIScene(Object *parent)
    : Widgets::Scene(parent)
{
    start = new StartLayer(this);
    game = new GameLayer(this);
    gameOver = new GameOverLayer(this);
    pause = new PauseLayer(this);
    settings = new SettingsLayer(this);

    start->updateValues();
    start->show();

    auto gameContent = parentGameScene()->parentGameContent();
    listen(gameContent);

    gameContent->addOnSizeChangedListener([this](int width, int height) {
        setSize(width, height);
    });
}

void GameUIScene::setSize(int width, int height) {
    Widgets::Scene::setSize(width, height);
    m_onSizeChanged.notify(width, height);
}

void GameUIScene::reset() {
    game->reset();
    start->updateValues();
}

GameScene* GameUIScene::parentGameScene() const {
    return assert_cast<GameScene*>(getParentScene());
}

MenuLayer* GameUIScene::firstMenuLayer() const {
    for (int i = 1; i < getLayersCount(); ++i)
        if (auto menuLayer = dynamic_cast<MenuLayer*>(layerAt(i)); menuLayer)
            return menuLayer;
    return nullptr;
}

Subscription<int, int> GameUIScene::addOnSizeChangedListener(const Observer<int, int> &listener) {
    return m_onSizeChanged.subscribe(listener);
}
} // UI
