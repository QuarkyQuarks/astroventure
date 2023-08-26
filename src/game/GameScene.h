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

    /**
     * Triggers a game reset.
     * @note This function must be called from the main rendering thread.
     */
    void triggerReset();

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

    /**
     * The event `onReset` is triggered by the `triggerReset` function.
     * @param listener The listener function to be added.
     * @return Subscription object representing the added listener.
     */
    Subscription<> addOnResetListener(const Observer<> &listener);

    /**
     * The event `onResetCompleted` is triggered when the reset is completed.
     * @param listener The listener function to be added.
     * @return Subscription object representing the added listener.
     */
    Subscription<> addOnResetCompletedListener(const Observer<> &listener);

    /**
     * Use this function to declare that the reset of some object begins.
     * Must be called in an on reset listener.
     * @note This function must be called from the main rendering thread.
     */
    void beginReset();

    /**
     * Use this function to indicate that the reset of some object has ended.
     * This function triggers a check to determine if the number of `beginReset`
     * calls is equal to the number of `endReset` calls. If they are equal, the
     * `onResetCompleted` event will be triggered.
     * @note This function may be called from any thread.
     */
    void endReset();

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

    void resetProgress();

private:
    ObservableInt m_score;
    ObservableInt m_crystals;

private:
    Subject<> m_onReset;
    Subject<> m_onResetCompleted;
    std::atomic_int m_resetStatus;

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
};

#endif //ASTROVENTURE_GAMESCENE_H
