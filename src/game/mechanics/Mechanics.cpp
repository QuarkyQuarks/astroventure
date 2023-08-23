#include "Mechanics.h"
#include "game/Cameraman.h"
#include "game/math/VecTools.h"
#include "game/GameScene.h"
#include "ODE.h"

#include <algine/core/log/Log.h>
#include <algine/core/log/logger/glm.h>

#include <glm/gtc/matrix_transform.hpp>

#define LOG_TAG "Mechanics"

namespace mechanics {
Mechanics::Mechanics(GameScene &scene)
    : m_state(State::Ground),
      m_bound(),
      m_scene(scene),
      m_trajectory()
{
    m_bound = mapScreenToWorld(
        glm::perspective(Cameraman::FieldOfView, 0.52f, Cameraman::Near, Cameraman::Far),
        {1.0f, 1.0f},
        -Cameraman::GamePos.z);

    step_ptr = nullptr;
}

bool Mechanics::isOnGround() {
    return m_state == State::Ground;
}

bool Mechanics::isDestroyed() {
    return m_state == State::Destroyed;
}

Subscription<> Mechanics::addOnLaunchListener(const Observer<> &listener) {
    return m_onLaunch.subscribe(listener);
}

Subscription<Planet*> Mechanics::addOnLandingListener(const Observer<Planet*> &listener) {
    return m_onLanding.subscribe(listener);
}

Subscription<Planet*> Mechanics::addOnGroundListener(const Observer<Planet*> &listener) {
    return m_onGround.subscribe(listener);
}

Subscription<> Mechanics::addOnDestroyedListener(const Observer<> &listener) {
    return m_onDestroyed.subscribe(listener);
}

void Mechanics::setBound(const vec2 &bound) {
    m_bound = bound;
}

void Mechanics::update() {
    if (step_ptr != nullptr) {
        (this->*step_ptr)();

        auto spacecraft = m_scene.getSpacecraft();
        spacecraft->translate();
        spacecraft->rotate();
        spacecraft->transform();

        // TODO: move this condition to flight() ?
        if (!inBounds(spacecraft->getPos())) {
            m_onDestroyed.notify();
            m_state = State::Destroyed;
        }
    }
}

void Mechanics::launch() {
    Log::debug(LOG_TAG, __func__);

    auto spacecraft = m_scene.getSpacecraft();
    spacecraft->setVelocity({0, 0, 0});
    spacecraft->setParent(&m_scene);

    // this method leaves capacity of the vector unchanged
    m_trajectory.points.clear();
    m_trajectory.velocity.clear();
    m_trajectory.landingPlanet = nullptr;
    m_trajectory.flightStart = 0;

    step_ptr = &Mechanics::takeOff;
    m_state = State::Launch;

    m_onLaunch.notify();
}

void Mechanics::takeOff() {
    Log::debug(LOG_TAG) << __func__;
    auto stepInterval = m_scene.getFrameTimeSec();
    auto spacecraft = m_scene.getSpacecraft();
    auto &velocity = spacecraft->getVelocity();

    vec3 deltaPos {0, 0, 0};
    deltaPos.x = ODE::linear(velocity.x, stepInterval);
    deltaPos.y = ODE::linear(velocity.y, stepInterval);
    spacecraft->changePos(deltaPos);

    const auto startingAngle = adjustedAngle<num_t>(spacecraft->getRoll() + glm::pi<num_t>() / 2.0);
    constexpr num_t accelScalar = 5.5;
    const vec2 acceleration {accelScalar * std::cos(startingAngle), accelScalar * std::sin(startingAngle)};

    vec3 deltaVel {0, 0, 0};
    deltaVel.x = ODE::linear(acceleration.x, stepInterval);
    deltaVel.y = ODE::linear(acceleration.y, stepInterval);
    spacecraft->changeVelocity(deltaVel);

    auto &planetPos = m_scene.getPlanets()[0]->getPos();
    auto distance = glm::distance(spacecraft->getPos(), planetPos);
    constexpr num_t takeOffDist = 0.21;

    if (distance >= takeOffDist && !(startingAngle < glm::pi<num_t>() * 2 && startingAngle > glm::pi<num_t>())) {
        trajectoryCalc();
        step_ptr = &Mechanics::flight;
    }
}

bool Mechanics::inBounds(const vec2 &pos) const {
    return std::abs(pos.x) <= m_bound.x && std::abs(pos.y) <= m_bound.y;
}

void Mechanics::trajectoryCalc() {
    auto spacecraft = m_scene.getSpacecraft();

    vec2 velocity = spacecraft->getVelocity();
    vec2 position = spacecraft->getPos();

    m_trajectory.points.emplace_back(position);
    m_trajectory.velocity.emplace_back(velocity);

    num_t E = 0.01;
    int closestPlanetId = 0;

    // we are calculating points until our virtual spacecraft encounters the bounds
    // (it happens even when we land on the planet);
    while (inBounds(position)) {
        auto velocity0 = velocity;
        auto &planets = m_scene.getPlanets();

        for (int i = 1; i < planets.size(); ++i) {
            auto planet = planets[i];
            auto planetPos = vec2(planet->getPos());

            const num_t radius = planet->getOrbit()->getRadius() + spacecraft->getHeight() / 2;
            num_t distToPlanet = glm::distance(position, planetPos);
            num_t minDistToPlanet = glm::distance(m_trajectory.points[closestPlanetId], planetPos);

            constexpr num_t margin = 0.02;

            if (distToPlanet <= (radius + margin)) {
                E = 0.001;
            }

            velocity.x += ODE::gravity<&vec2::x, 2>(position - planetPos, E);
            velocity.y += ODE::gravity<&vec2::y, 2>(position - planetPos, E);

            if (distToPlanet < minDistToPlanet ) {
                closestPlanetId = (int) m_trajectory.points.size();
            }

            if (distToPlanet <= radius && !m_trajectory.landingPlanet) {
                m_trajectory.landingPlanet = planet;
            }
        }

        E = 0.01;
        position.x += ODE::linear(velocity0.x, E);
        position.y += ODE::linear(velocity0.y, E);

        m_trajectory.points.emplace_back(position);
        m_trajectory.velocity.emplace_back(velocity);
    }

    // after all those iterations we proceed to process calculated points based on a some conditions
    // if there were no point at same minimal distance near one of the planets, trajectory don't need to be modified
    // otherwise, we erase all the points after this critical point - we construct landing trajectory

    if (m_trajectory.landingPlanet) {
        Log::debug(LOG_TAG) << closestPlanetId << "   " << m_trajectory.points.size() - 1 << Log::endl;

        m_trajectory.points.erase(m_trajectory.points.begin() + closestPlanetId, m_trajectory.points.end());
        // TODO: construct some artificial points to smooth out docking with orbit
        // TODO: erase unused velocities?
    }

    m_trajectory.flightStart = static_cast<num_t>(Engine::timeFromStart()) / 1000.0;
}

void Mechanics::flight() {
    //motion
    auto currentTime = static_cast<num_t>(Engine::timeFromStart()) / 1000.0;
    num_t timeFromStart = currentTime - m_trajectory.flightStart;

    constexpr num_t E = 0.01; // change for non-constant case

    auto id = static_cast<int>(timeFromStart / E);
    num_t timeRatio = (timeFromStart - static_cast<num_t>(id) * E) / E;

    motion(id, timeRatio);

    // rotation
    auto planets = m_scene.getPlanets();
    auto spacecraft = m_scene.getSpacecraft();

    constexpr num_t landingRadius = 0.3;

    vec2 impactFromPlanet[2];
    vec2 distToPlanet[2];

    for (int i = 1; i < m_scene.getPlanets().size(); ++i) {
        const num_t radius = planets[i]->getOrbit()->getRadius() + spacecraft->getHeight() / 2;

        distToPlanet[i - 1] = spacecraft->getPos() - planets[i]->getPos();
        num_t k = (glm::length(distToPlanet[i - 1]) - radius) / (landingRadius - radius);
        k = k >= 0 ? k : 0.0;
        impactFromPlanet[i - 1] = rotation(distToPlanet[i - 1], k);
    }

    auto pointsSize = m_trajectory.points.size();
    auto planet = m_trajectory.landingPlanet;

    if (id >= pointsSize - 1 && planet) {
        auto dir = glm::normalize(impactFromPlanet[planet->getIndex() - 1]); // TODO rename
        spacecraft->setRoll(-angleBetweenVectors(dir, {0.0f, 1.0f}));
    } else {
        auto d1 = glm::length(distToPlanet[0]);
        auto d2 = glm::length(distToPlanet[1]);
        num_t ratio = d1 / (d1 + d2);

        auto mixed = glm::normalize(glm::mix(impactFromPlanet[0], impactFromPlanet[1], ratio));
        spacecraft->setRoll(-angleBetweenVectors(mixed, {0.0f, 1.0f}));
    }

    const num_t radius = planets[1]->getOrbit()->getRadius() + spacecraft->getHeight() / 2;

    if (planet && glm::distance(spacecraft->getPos(), planet->getPos()) <= radius) {
        orbitDocking();
    }
}

vec2 Mechanics::rotation(vec2 distToPlanet, num_t k) const {
    constexpr num_t epsilon = 10E-8;

    auto spacecraft = m_scene.getSpacecraft();

    vec2 direction = glm::normalize(spacecraft->getVelocity());
    auto impact = glm::normalize(distToPlanet);

    if (std::abs(k - 1) <= epsilon) { // k == 1
        return impact;
    } else {
        if (k > epsilon && k < 1) // k > 0 TODO
            return glm::mix(impact, direction, k);
        return direction;
    }
}

void Mechanics::motion(int id, num_t ratio) {
    auto &points = m_trajectory.points;
    auto &velocities = m_trajectory.velocity;
    auto pointsSize = points.size();

    auto spacecraft = m_scene.getSpacecraft();

    // extrapolation of the curve if the number of m_trajectory points is not enough
    // mix is performed on a segment 'delta', delta is proportional to the current speed
    // and to factor that is determined by the id
    if (id >= pointsSize - 1) {
        vec2 delta = (points[pointsSize - 1] - points[pointsSize - 2]) * static_cast<num_t>((1 + id - (pointsSize - 1)));
        spacecraft->setPos({glm::mix(points[pointsSize - 1], points[pointsSize - 1] + delta, ratio), 0});
        return;
    }

    //linear interpolation is used when we don't have enough points to use cubic
    if (id == 0 || id >= pointsSize - 3) { // TODO: check correctness
        spacecraft->setPos({glm::mix(points[id], points[id + 1], ratio), 0});
        spacecraft->setVelocity(glm::mix(velocities[id], velocities[id + 1], ratio));
    } else {
        spacecraft->setPos({mixCubic(points[id - 1], points[id], points[id + 1], points[id + 2], ratio), 0});
        spacecraft->setVelocity(mixCubic(velocities[id - 1], velocities[id], velocities[id + 1], velocities[id + 2], ratio));
    }
}

void Mechanics::orbitDocking() {
    auto spacecraft = m_scene.getSpacecraft();
    auto planet = m_trajectory.landingPlanet;

    vec2 dir = glm::normalize(spacecraft->getPos() - planet->getPos());
    spacecraft->setRoll(-PI / 2 + angleBetweenVectors({1.0, 0.0}, dir));

    spacecraft->setVelocity({-std::cos(spacecraft->getRoll() + PI/2), -std::sin(spacecraft->getRoll() + PI/2), 0});

    auto orbit = planet->getOrbit();
    int platformId = orbit->platformAtAbs(angleBetweenVectors({1.0, 0.0}, dir));

    if (platformId != -1) {
        orbit->fall(platformId);
        step_ptr = &Mechanics::landing;
        m_onLanding.notify(planet);
        m_state = State::Landing;
    } else {
        m_onDestroyed.notify();
        m_state = State::Destroyed;
    }
}

void Mechanics::landing() {
    auto spacecraft = m_scene.getSpacecraft();
    auto planet = m_trajectory.landingPlanet;

    auto stepInterval = m_scene.getFrameTimeSec();
    const num_t angularVelocity = planet->getAngularVelocity();

    const vec2 pos = spacecraft->getPos() - planet->getPos();

    const num_t deltaAngle = adjustedAngle(stepInterval * angularVelocity);

    const num_t x = pos.x * std::cos(deltaAngle) - pos.y * std::sin(deltaAngle);
    const num_t y = pos.x * std::sin(deltaAngle) + pos.y * std::cos(deltaAngle);

    spacecraft->setRoll(spacecraft->getRoll() + deltaAngle);
    spacecraft->setPos({planet->getX() + x, planet->getY() + y, 0.0});

    //radial translation
    vec3 deltaPos {0, 0, 0};
    deltaPos.x = ODE::linear(spacecraft->getVelocity().x, stepInterval);
    deltaPos.y = ODE::linear(spacecraft->getVelocity().y, stepInterval);
    spacecraft->changePos(deltaPos);

    constexpr num_t accelScalar = -1.5;
    num_t roll = spacecraft->getRoll() + PI / 2;
    const vec2 acceleration {accelScalar * cosf(roll), accelScalar * sinf(roll)};

    vec3 deltaVel {0, 0, 0};
    deltaVel.x = ODE::linear(acceleration.x, stepInterval);
    deltaVel.y = ODE::linear(acceleration.x, stepInterval);
    spacecraft->changeVelocity(deltaVel);

    constexpr num_t planetRadius = 0.1; // TODO move to planet object

    if (glm::distance(spacecraft->getPos(), planet->getPos()) <= planetRadius) {
        //TODO
        vec2 dir = glm::normalize(spacecraft->getPos() - planet->getPos());
        spacecraft->setRoll(-PI / 2 + angleBetweenVectors({1.0, 0.0}, dir));
        num_t angle = spacecraft->getRoll() + PI / 2;
        vec3 final_pos = planet->getPos() + glm::vec3(planetRadius * std::cos(angle), planetRadius * std::sin(angle), 0);
        spacecraft->setPos(final_pos);

        spacecraft->translate();
        spacecraft->rotate();
        spacecraft->transform();

        spacecraft->setParent(planet->getOrbit());

        m_onGround.notify(planet);
        m_state = State::Ground;
        step_ptr = nullptr;
    }
}
}