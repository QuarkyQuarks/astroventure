#include "GameController.h"
#include "game/GameScene.h"

#include <algine/core/log/Log.h>
#include <algine/core/log/logger/glm.h>

#define LOG_TAG "GameController"

using namespace algine;

GameController::GameController(GameScene &gameScene)
    : Controller(gameScene),
      eventHandler(&GameController::launchEventHandler),
      scroll(nullptr),
      m_scrollDetails()
{
    auto &mechanics = gameScene.getMechanics();

    mechanics.addOnLaunchListener([this] {
        blockLaunch();
    });

    mechanics.addOnLandingListener([this](Planet *planet) {
        triggerScroll(planet);
        generatePlanets(planet->getIndex());
    });

    gameScene.getResetAction().addOnTriggerListener([this] {
        reset();
    });
}

void GameController::update() {
    m_gameScene.getMechanics().update();

    if (scroll != nullptr) {
        (this->*scroll)();
    }
}

void GameController::event(const Event &event) {
    Log::debug(LOG_TAG, __func__);

    if (eventHandler != nullptr) {
        (this->*eventHandler)(event);
    }
}

void GameController::blockLaunch() {
    eventHandler = nullptr;
}

void GameController::unblockLaunch() {
    eventHandler = &GameController::launchEventHandler;
}

void GameController::launchEventHandler(const algine::Event &event) {
    if (event.getId() != Event::Id::Click)
        return;
    m_gameScene.getMechanics().launch();
}

void GameController::generatePlanets(int count) {
    acquireScrollLock();

    auto &planets = m_gameScene.getPlanets();
    auto visiblePlanetCount = static_cast<int>(planets.size());
    auto xSign = -static_cast<int>(glm::sign(planets.back()->getX()));

    for (int index = visiblePlanetCount; index < visiblePlanetCount + count; ++index) {
        PlanetManager::Properties properties = {
            {PlanetManager::Property::PositionDetails, PlanetManager::PositionDetails {.index = index, .xSign = xSign}}
        };

        m_gameScene.getPlanetManager()->getAsync([=, this](Planet *planet) {
            auto &planets = m_gameScene.planets();
            planets[index] = planet;

            if (planets.count() == visiblePlanetCount + count) {
                releaseScrollLock();
            }
        }, PlanetManager::Flag::CalcPosition, properties);
    }
}

void GameController::triggerScroll(Planet *dst) {
    constexpr long waitMs = 50;

    m_scrollDetails = {
        .startTimeMs = Engine::timeFromStart() + waitMs,
        .totalDistance = dst->getY() - m_gameScene.getPlanets()[0]->getY(),
        .dst = dst
    };

    scroll = &GameController::beforeScroll;
}

void GameController::acquireScrollLock() {
    ++m_scrollDetails.lockCount;
}

void GameController::releaseScrollLock() {
    --m_scrollDetails.lockCount;
}

bool GameController::isScrollLocked() const {
    return m_scrollDetails.lockCount != 0;
}

void GameController::beforeScroll() {
    const long time = Engine::timeFromStart();
    const long deltaTime = time - m_scrollDetails.startTimeMs;

    if (deltaTime < 0)
        return;

    if (!isScrollLocked()) {
        scroll = &GameController::onScroll;
        m_scrollDetails.startTimeMs = time;
    }
}

void GameController::onScroll() {
    constexpr long duration = 1000;

    const long time = Engine::timeFromStart();
    const long deltaTime = time - m_scrollDetails.startTimeMs;

    auto f = [](float x) -> float {
        return -std::pow(x - 1, 4.0f) + 1;
    };

    float posX = static_cast<float>(deltaTime) / static_cast<float>(duration);

    if (posX >= 1.0f) {
        scroll = nullptr;
        afterScroll();
        return;
    }

    float posY = f(posX);
    float distance = glm::mix(0.0f, m_scrollDetails.totalDistance, posY);

    glm::vec3 delta {0, m_scrollDetails.currentDistance - distance, 0};

    m_scrollDetails.currentDistance = distance;

    for (auto planet : m_gameScene.planets()) {
        planet->changeClusterPos(delta);
    }
}

void GameController::afterScroll() {
    // remove invisible planets & update indices
    auto &planets = m_gameScene.planets();
    auto dstIndex = m_scrollDetails.dst->getIndex();

    for (int i = 0; i < dstIndex; ++i)
        m_gameScene.getPlanetManager()->done(planets[i]);

    planets.erase(planets.begin(), planets.begin() + dstIndex);

    for (auto planet : planets)
        planet->setIndex(planet->getIndex() - dstIndex);

    // set final pos
    glm::vec3 delta {0, m_scrollDetails.currentDistance - m_scrollDetails.totalDistance, 0};

    for (auto planet : m_gameScene.planets()) {
        planet->changeClusterPos(delta);
    }

    unblockLaunch();
}

void GameController::reset() {
    m_gameScene.getResetAction().lock();

    unblockLaunch();

    resetPlanets();
}

void GameController::resetPlanets() {
    // since the spacecraft is attached to an orbit,
    // and it will be deleted, we need to prevent
    // spacecraft deletion
    m_gameScene.getSpacecraft()->setParent(&m_gameScene);

    auto planetManager = m_gameScene.getPlanetManager();
    auto &planets = m_gameScene.planets();
    auto planetCount = planets.size();

    for (auto planet : planets)
        planetManager->done(planet);

    planets.erase(planets.begin(), planets.end());

    for (int i = 0; i < planetCount; ++i) {
        int flags = PlanetManager::Flag::CalcPosition;

        if (i == 0)
            flags |= PlanetManager::DisableOrbit;

        PlanetManager::Properties properties = {
            {PlanetManager::Property::PositionDetails, PlanetManager::PositionDetails {.index = i, .xSign = 1}}
        };

        planetManager->getAsync([=, this](Planet *planet) {
            planet->setParent(&m_gameScene);

            auto &planets = m_gameScene.planets();
            planets[i] = planet;

            if (planets.count() == planetCount) {
                resetSpacecraft();
                resetCamera();
                m_gameScene.getResetAction().unlock();
            }
        }, flags, properties);
    }
}

void GameController::resetSpacecraft() {
    auto &planets = m_gameScene.getPlanets();
    auto spacecraft = m_gameScene.getSpacecraft();
    spacecraft->attachTo(planets[0]);
}

void GameController::resetCamera() {
    // must be called after the planets have been generated
    // because the cameraman's home animation depends on the
    // position of the zeroth planet
    m_gameScene.getCameraman().startAnimation(Cameraman::Animation::Home);
}
