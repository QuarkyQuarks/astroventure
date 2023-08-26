#include "GameScene.h"
#include "GameContent.h"
#include "planet/Planet.h"
#include "spacecraft/Spacecraft.h"
#include "controller/StartController.h"

#include <algine/core/assert_cast.h>
#include <algine/core/Engine.h>
#include <algine/core/Framebuffer.h>

STATIC_INITIALIZER_IMPL(GameScene) {
    Lua::addTypeLoader("GameScene", [](sol::environment &env) {
        if (env["GameScene"].valid())
            return;

        auto usertype = env.new_usertype<GameScene>("GameScene");
        usertype["getColorSchemeManager"] = &GameScene::getColorSchemeManager;
        usertype["cast"] = [](Scene *scene) {
            return dynamic_cast<GameScene*>(scene);
        };
    });
}

GameScene::GameScene(GameContent *parent)
    : Scene(parent),
      m_score(0),
      m_crystals(0),
      m_renderer(new GameRenderer(this)),
      m_cameraman(*this),
      m_mechanics(*this),
      m_crystalParticles(new CrystalParticleSystem(this)),
      m_settingsManager(*this),
      m_audioManager(*this),
      m_colorSchemeManager(*this),
      m_planetManager(new PlanetManager(this)),
      m_spacecraftManager(new SpacecraftManager(this)),
      m_spacecraft(),
      m_controller(),
      m_planets(3),
      m_prevFrameTime(Engine::timeFromStart()),
      m_prevDeltaFrameTime(0)
{
    m_mechanics.addOnGroundListener([this](Planet*) {
        ++m_score;
    });

    m_crystalParticles->addOnParticleRemovedListener([this] {
        ++m_crystals;
    });
}

void GameScene::render() {
    auto currentFrameTime = Engine::timeFromStart();
    m_prevDeltaFrameTime = static_cast<int>(currentFrameTime - m_prevFrameTime);
    m_prevFrameTime = currentFrameTime;

    Framebuffer::setClearColor(0.0f, 0.0f, 0.0f);
    m_cameraman.animate();
    m_controller->update();
    m_renderer->render();
}

void GameScene::pause() {

}

LoaderConfig GameScene::resourceLoaderConfig() {
    return LoaderConfig::create<GameScene>({
        .loader = [this]() { loadResources(); },
        .children = {
            m_renderer,
            &m_audioManager, m_planetManager,
            m_crystalParticles, &m_cameraman
        },
        .dependsOn = {m_planetManager}
    });
}

GameContent* GameScene::parentGameContent() const {
    return assert_cast<GameContent*>(getParent());
}

void GameScene::setController(Controller *controller) {
    m_controller = controller;
}

Controller* GameScene::getController() const {
    return m_controller;
}

Cameraman& GameScene::getCameraman() {
    return m_cameraman;
}

mechanics::Mechanics& GameScene::getMechanics() {
    return m_mechanics;
}

Spacecraft* GameScene::getSpacecraft() const {
    return m_spacecraft;
}

const PlanetArray& GameScene::getPlanets() const {
    return m_planets;
}

PlanetArray& GameScene::planets() {
    return m_planets;
}

CrystalParticleSystem& GameScene::getCrystalParticles() {
    return *m_crystalParticles;
}

SettingsManager& GameScene::getSettingsManager() {
    return m_settingsManager;
}

ColorSchemeManager& GameScene::getColorSchemeManager() {
    return m_colorSchemeManager;
}

PlanetManager* GameScene::getPlanetManager() const {
    return m_planetManager;
}

ObservableInt& GameScene::getScore() {
    return m_score;
}

ObservableInt& GameScene::getCrystals() {
    return m_crystals;
}

float GameScene::getFrameTimeSec() const {
    return static_cast<float>(m_prevDeltaFrameTime) / 1000.0f;
}

int GameScene::getFrameTime() const {
    return m_prevDeltaFrameTime;
}

void GameScene::loadResources() {
    constexpr int planetCount {3};
    int planetCompletionCount {0};
    bool isSpacecraftLoaded {false};

    std::condition_variable cv;
    std::mutex mutex;

    for (int i = 0; i < planetCount; ++i) {
        int flags = PlanetManager::Flag::CalcPosition;

        if (i == 0)
            flags |= PlanetManager::DisableOrbit;

        PlanetManager::Properties properties = {
            {PlanetManager::Property::PositionDetails, PlanetManager::PositionDetails {.index = i, .xSign = 1}}
        };

        m_planetManager->getAsync([&, i](Planet *planet) {
            planet->setParent(this);

            {
                std::lock_guard locker(mutex);
                ++planetCompletionCount;
                m_planets[i] = planet;
            }

            cv.notify_one();
        }, flags, properties);
    }

    m_spacecraftManager->getAsync(m_settingsManager.getCurrentSpacecraftId(), [&](Spacecraft *spacecraft) {
        {
            std::lock_guard locker(mutex);
            isSpacecraftLoaded = true;
            m_spacecraft = spacecraft;
        }

        cv.notify_one();
    });

    {
        std::unique_lock lock(mutex);
        cv.wait(lock, [&planetCompletionCount, &isSpacecraftLoaded] {
            return planetCompletionCount == planetCount && isSpacecraftLoaded;
        });
    }

    toInitialPositions();
}

void GameScene::toInitialPositions() {
    // move the spacecraft to the zeroth planet
    auto planet = m_planets[0];
    m_spacecraft->attachTo(planet);
}
