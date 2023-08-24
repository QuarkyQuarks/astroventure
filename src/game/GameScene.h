#ifndef ASTROVENTURE_GAMESCENE_H
#define ASTROVENTURE_GAMESCENE_H

#include <algine/core/scene/Scene.h>

#include <tulz/observer/ObservableNumber.h>
#include <tulz/static_initializer.h>

#include <memory>

#include "loader/Loadable.h"
#include "render/GameRenderer.h"
#include "ui/GameUIScene.h"
#include "audio/AudioPlayer.h"
#include "audio/AudioManager.h"
#include "mechanics/Mechanics.h"
#include "planet/PlanetManager.h"
#include "planet/CrystalParticleSystem.h"
#include "spacecraft/SpacecraftManager.h"
#include "scheme/ColorSchemeManager.h"
#include "controller/Controller.h"
#include "SettingsManager.h"
#include "Cameraman.h"

using namespace algine;
using namespace tulz;

class GameContent;

class Planet;
class Spacecraft;

class GameScene: public Scene, public Loadable {
    STATIC_INITIALIZER_DECL

public:
    explicit GameScene(GameContent *parent);

    void render() override;

    void pause();

    LoaderConfig resourceLoaderConfig() override;

    GameContent* parentGameContent() const;

    void setController(std::unique_ptr<Controller> controller);
    Controller* getController() const;

    Cameraman& getCameraman();
    mechanics::Mechanics& getMechanics();

    Spacecraft* getSpacecraft() const;
    const std::vector<Planet*>& getPlanets() const;
    CrystalParticleSystem& getCrystalParticles();

    SettingsManager& getSettingsManager();
    ColorSchemeManager& getColorSchemeManager();

    ObservableInt& getScore();
    ObservableInt& getCrystals();

    /**
     * @return previous frame rendering time in seconds
     */
    float getFrameTimeSec() const;

    /**
     * @return previous frame rendering time in milliseconds
     */
    int getFrameTime() const;

private:
    /**
     * Triggers 3 planets generation & spacecraft loading
     */
    void loadResources();

    void toInitialPositions();

private:
    ObservableInt m_score;
    ObservableInt m_crystals;

private:
    Cameraman m_cameraman;
    mechanics::Mechanics m_mechanics;
    CrystalParticleSystem *m_crystalParticles;

private:
    SettingsManager m_settingsManager;
    AudioManager m_audioManager;
    ColorSchemeManager m_colorSchemeManager;
    PlanetManager *m_planetManager;
    SpacecraftManager *m_spacecraftManager;

private:
    GameRenderer *m_renderer;

private:
    Spacecraft *m_spacecraft;
    std::vector<Planet*> m_planets;

private:
    std::unique_ptr<Controller> m_controller;

private:
    long m_prevFrameTime;
    int m_prevDeltaFrameTime;
};

#endif //ASTROVENTURE_GAMESCENE_H
