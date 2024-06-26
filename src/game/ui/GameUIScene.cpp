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

GameScene* GameUIScene::parentGameScene() const {
    return assert_cast<GameScene*>(getParentScene());
}

GameRenderer* GameUIScene::parentGameRenderer() const {
    auto parent = findParent<GameRenderer*>();
    assert(parent != nullptr);
    return parent;
}

Subscription<int, int> GameUIScene::addOnSizeChangedListener(const Observer<int, int> &listener) {
    return m_onSizeChanged.subscribe(listener);
}

void GameUIScene::onShow() {
    start->show();
}
} // UI
