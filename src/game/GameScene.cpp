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
      m_audioManager(*this),
      m_colorSchemeManager(*this),
      m_planetManager(new PlanetManager(this)),
      m_spacecraftManager(new SpacecraftManager(this)),
      m_spacecraft(),
      m_controller(std::make_unique<StartController>(*this)),
      m_planets(3),
      m_prevFrameTime(Engine::timeFromStart()),
      m_prevDeltaFrameTime(0)
{
    m_settingsManager.load();

    m_mechanics.addOnGroundListener([this](Planet*) {
        ++m_score;
    });

    m_crystalParticles->addOnParticleRemovedListener([this] {
        ++m_crystals;
    });
}

void GameScene::init() {

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

void GameScene::setController(std::unique_ptr<Controller> controller) {
    m_controller = std::move(controller);
}

Controller* GameScene::getController() const {
    return m_controller.get();
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

const std::vector<Planet*>& GameScene::getPlanets() const {
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
        m_planetManager->getAsync([&, i](Planet *planet) {
            planet->setIndex(i);

            {
                std::lock_guard locker(mutex);
                ++planetCompletionCount;
                m_planets[i] = planet;
            }

            cv.notify_one();
        }, (i == 0 ? PlanetManager::Flag::DisableOrbit : PlanetManager::Flag::None));
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
    constexpr float planet_x = 0.125f; // horizontal distance between two neighboring planets
    constexpr float planet_y = 0.45f; // vertical distance between two neighboring planets
    constexpr float planet_z = 0.0f;

    // positions of planets:
    // 0: (-planet_x, -planet_y, planet_z)
    // 1: ( planet_x, 0.0f,      planet_z)
    // 2: (-planet_x, planet_y,  planet_z)
    // and so on

    for (auto planet : m_planets) {
        auto index = planet->getIndex();

        glm::vec3 initialPos {
            planet_x * ((index % 2 == 0) ? -1.0f : 1.0f),
            planet_y * static_cast<float>(index - 1),
            planet_z
        };

        planet->changeClusterPos(initialPos - planet->getPos());

        planet->setRoll(0);
        planet->rotate();

        planet->transform();
    }

    // move spacecraft to the zero planet
    m_spacecraft->setY(-planet_y);
    m_spacecraft->setX(-0.025); // TODO: hardcoded value, must be -planet_x + planet_radius
    m_spacecraft->setRoll(-PI / 2);
    m_spacecraft->setParent(m_planets[0]);
}
