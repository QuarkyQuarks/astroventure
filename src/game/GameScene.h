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
#include "planet/PlanetArray.h"
#include "spacecraft/SpacecraftManager.h"
#include "scheme/ColorSchemeManager.h"
#include "controller/Controller.h"
#include "action/StateAction.h"
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

    LoaderConfig resourceLoaderConfig() override;

    GameContent* parentGameContent() const;

    void setController(Controller *controller);
    Controller* getController() const;

    Cameraman& getCameraman();
    mechanics::Mechanics& getMechanics();

    Spacecraft* getSpacecraft() const;
    const PlanetArray& getPlanets() const;
    PlanetArray& planets();
    CrystalParticleSystem& getCrystalParticles();

    SettingsManager& getSettingsManager();
    ColorSchemeManager& getColorSchemeManager();
    PlanetManager* getPlanetManager() const;

    ObservableInt& getScore();
    ObservableInt& getCrystals();

    StateAction& getResetAction();
    StateAction& getPauseAction();
    StateAction& getResumeAction();

    /**
     * Adds a listener that will be called on each frame rendering.
     * @param listener The listener to be added.
     * @return Subscription object representing the added listener.
     */
    Subscription<> addOnTickListener(const Observer<> &listener);

    /**
     * @return previous frame rendering time in seconds
     */
    float getFrameTimeSec() const;

    /**
     * @return previous frame rendering time in milliseconds
     */
    int getFrameTime() const;

    /**
     * Sets the time scale. The default value is 1.0.
     * @param scale The non-negative scale value.
     */
    void setTimeScale(float scale);

    float getTimeScale() const;

    /**
     * Starts a time scaling animation.
     * @param dstScale The destination time scale value.
     * @param durationMs The animation duration in milliseconds.
     * @param callback The callback that will be called
     * when the animation is finished.
     */
    void startTimeScaling(float dstScale, int durationMs = 200, std::function<void()> callback = {});

    /**
     * The same as `getFrameTimeSec` but scaled.
     * @return previous frame rendering time in seconds
     * with applied time scaling.
     */
    float getScaledFrameTimeSec() const;

    /**
     * The same as `getFrameTime` but scaled.
     * @return previous frame rendering time in milliseconds
     * with applied time scaling.
     */
    float getScaledFrameTime() const;

private:
    /**
     * Triggers 3 planets generation & spacecraft loading
     */
    void loadResources();

    void toInitialPositions();

    void resetProgress();

private:
    ObservableInt m_score;
    ObservableInt m_crystals;

private:
    StateAction m_reset;
    StateAction m_pause;
    StateAction m_resume;

private:
    Subject<> m_onTick;

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
    Controller *m_controller;

private:
    GameRenderer *m_renderer;

private:
    Spacecraft *m_spacecraft;
    PlanetArray m_planets;

private:
    long m_prevFrameTime;
    int m_prevDeltaFrameTime;
    float m_timeScale;
};

#endif //ASTROVENTURE_GAMESCENE_H
