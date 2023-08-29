#include "GameScene.h"
#include "GameContent.h"
#include "planet/Planet.h"
#include "spacecraft/Spacecraft.h"
#include "controller/StartController.h"

#include <algine/core/assert_cast.h>
#include <algine/core/Engine.h>
#include <algine/core/Framebuffer.h>
#include <algine/core/Window.h>

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
      m_reset(*this),
      m_pause(*this),
      m_resume(*this),
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
      m_prevDeltaFrameTime(0),
      m_timeScale(1.0f)
{
    m_mechanics.addOnGroundListener([this](Planet*) {
        ++m_score;
    });

    m_crystalParticles->addOnParticleRemovedListener([this] {
        ++m_crystals;
    });

    m_reset.addOnTriggerListener([this] {
        resetProgress();
        setTimeScale(1.0f);
    });

    m_pause.addOnTriggerListener([this] {
        m_pause.lock();

        startTimeScaling(0.0f, 400, [this] {
            m_pause.unlock();
        });
    });

    m_resume.addOnCompletedListener([this] {
        startTimeScaling(1.0f, 400);
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

    m_onTick.notify();
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

StateAction& GameScene::getResetAction() {
    return m_reset;
}

StateAction& GameScene::getPauseAction() {
    return m_pause;
}

StateAction& GameScene::getResumeAction() {
    return m_resume;
}

Subscription<> GameScene::addOnTickListener(const Observer<> &listener) {
    return m_onTick.subscribe(listener);
}

void GameScene::resetProgress() {
    m_settingsManager.saveProgress();
    m_score = ObservableInt {0};
    m_crystals = ObservableInt {0};
}

float GameScene::getFrameTimeSec() const {
    return static_cast<float>(m_prevDeltaFrameTime) / 1000.0f;
}

int GameScene::getFrameTime() const {
    return m_prevDeltaFrameTime;
}

void GameScene::setTimeScale(float scale) {
    assert(scale >= 0.0f);
    m_timeScale = scale;
}

float GameScene::getTimeScale() const {
    return m_timeScale;
}

void GameScene::startTimeScaling(float dstScale, int durationMs, std::function<void()> callback) {
    auto startTime = Engine::timeFromStart();

    auto f = [](float t) -> float {
        return std::pow(t, 1.5f);
    };

    auto f_inv = [](float t) -> float {
        return std::pow(t, 1.0f / 1.5f);
    };

    auto srcX = f_inv(getTimeScale());
    auto dstX = f_inv(dstScale);

    auto sub = new Subscription<>();
    *sub = addOnTickListener([=, this, callback = std::move(callback)] {
        auto elapsed = static_cast<float>(Engine::timeFromStart() - startTime);
        float t = glm::clamp(elapsed / static_cast<float>(durationMs), 0.0f, 1.0f);

        auto x = glm::mix(srcX, dstX, t);
        auto y = f(x);

        setTimeScale(y);

        if (t == 1.0f) {
            if (callback != nullptr) {
                callback();
            }

            sub->unsubscribe();

            delete sub;
        }
    });
}

float GameScene::getScaledFrameTimeSec() const {
    return getFrameTimeSec() * m_timeScale;
}

float GameScene::getScaledFrameTime() const {
    return static_cast<float>(getFrameTime()) * m_timeScale;
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
