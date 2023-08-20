#ifndef ASTROVENTURE_GAMEUISCENE_H
#define ASTROVENTURE_GAMEUISCENE_H

#include <algine/core/widgets/Scene.h>

#include "StartLayer.h"
#include "GameLayer.h"
#include "game/ui/menu/GameOverLayer.h"
#include "game/ui/menu/PauseLayer.h"
#include "game/ui/menu/SettingsLayer.h"

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

    void reset();

    GameScene* parentGameScene() const;
    MenuLayer* firstMenuLayer() const;
};
} // UI

#endif //ASTROVENTURE_GAMEUISCENE_H
