#ifndef ASTROVENTURE_GAMEUISCENE_H
#define ASTROVENTURE_GAMEUISCENE_H

#include <algine/core/widgets/Scene.h>

#include "StartLayer.h"
#include "GameLayer.h"
#include "game/ui/menu/GameOverLayer.h"
#include "game/ui/menu/PauseLayer.h"
#include "game/ui/menu/SettingsLayer.h"

#include <tulz/observer/Subject.h>

class GameScene;

namespace UI {
class GameUIScene: public Widgets::Scene {
public:
    StartLayer *start {};
    GameLayer *game {};
    GameOverLayer *gameOver {};
    PauseLayer *pause {};
    SettingsLayer *settings {};

public:
    explicit GameUIScene(Object *parent);

    void setSize(int width, int height) override;

    void reset();

    GameScene* parentGameScene() const;
    MenuLayer* firstMenuLayer() const;

    tulz::Subscription<int, int> addOnSizeChangedListener(const tulz::Observer<int, int> &listener);

private:
    tulz::Subject<int, int> m_onSizeChanged;
};
} // UI

#endif //ASTROVENTURE_GAMEUISCENE_H
