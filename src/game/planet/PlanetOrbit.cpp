#include "PlanetOrbit.h"
#include "game/math/MathConstants.h"
#include "game/math/VecTools.h"
#include "game/GameScene.h"

#include <algine/core/assert_cast.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace algine;
using namespace tulz;

PlanetOrbit::Chunks::Chunks(PlanetOrbit &orbit): m_orbit(orbit) {}

PlanetOrbit::Chunks::Iterator PlanetOrbit::Chunks::begin() {
    return Iterator(Info(0, 0, &m_orbit));
}

PlanetOrbit::Chunks::Iterator PlanetOrbit::Chunks::end() {
    return Iterator(Info(m_orbit.getPlatformsCount(), 0, &m_orbit));
}

PlanetOrbit::Chunks::Iterator::Iterator(const Info &info): m_info(info) {}

PlanetOrbit::Chunks::Iterator& PlanetOrbit::Chunks::Iterator::operator++() {
    auto chunksCount = m_info.platform().chunks.size();

    if (m_info.chunkId() + 1 == chunksCount) {
        ++m_info.m_platformId;
        m_info.m_chunkId = 0;
    } else {
        ++m_info.m_chunkId;
    }

    return *this;
}

const PlanetOrbit::Chunks::Iterator PlanetOrbit::Chunks::Iterator::operator++(int) {
    auto it = *this;
    ++(*this);
    return it;
}

bool PlanetOrbit::Chunks::Iterator::operator==(Iterator other) const {
    return m_info == other.m_info;
}

bool PlanetOrbit::Chunks::Iterator::operator!=(Iterator other) const {
    return !(*this == other);
}

PlanetOrbit::Chunks::Iterator::reference PlanetOrbit::Chunks::Iterator::operator*() const {
    return const_cast<Info&>(m_info);
}

PlanetOrbit::PlanetOrbit(Array<Platform> platforms, Object *parent)
    : ClusterModel(SpaceModel::Type::PlanetOrbit, parent),
      m_platforms(std::move(platforms)),
      m_fallenChunks(0),
      m_radius(0.125)
{
    setState(State::AroundPlanet);
}

bool PlanetOrbit::isAroundPlanet() const {
    return m_state == State::AroundPlanet;
}

bool PlanetOrbit::isFalling() const {
    return m_state == State::Falling;
}

bool PlanetOrbit::isOnPlanet() const {
    return m_state == State::OnPlanet;
}

glm::mat4 PlanetOrbit::getChunkTransform(int platformId, int chunkId) {
    assert(m_step_ptr);
    return (this->*m_step_ptr)(platformId, chunkId);
}

void PlanetOrbit::setState(State state) {
    m_state = state;

    switch (m_state) {
        case State::AroundPlanet:
            m_step_ptr = &PlanetOrbit::aroundPlanet;
            break;
        case State::Falling:
            m_step_ptr = &PlanetOrbit::falling;
            break;
        case State::OnPlanet:
            m_step_ptr = &PlanetOrbit::onPlanet;
            break;
    }
}

glm::mat4 PlanetOrbit::chunkTransformMat(float radius, float roll, float scaleFactor) {
    glm::mat4 deltaTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(cosf(roll) * radius, sinf(roll) * radius, 0));
    glm::mat4 deltaRotation = glm::rotate(glm::mat4(1.0f), roll, {0.0f, 0.0f, 1.0f});

    if (scaleFactor != 1.0f) {
        glm::mat4 deltaScale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0, scaleFactor, 1.0f));
        return transformation() * deltaTranslation * deltaRotation * deltaScale;
    }

    return transformation() * deltaTranslation * deltaRotation;
}

glm::mat4 PlanetOrbit::aroundPlanet(int platformID, int chunkID) {
    float roll = getChunkRoll(platformID, chunkID);
    float radius = m_platforms[platformID].chunks[chunkID].radius;
    return chunkTransformMat(radius, roll, 1.0f);
}

void PlanetOrbit::fall(int touchdownPlatformId) {
    setState(State::Falling);

    auto spacecraft = parentGameScene()->getSpacecraft();
    glm::vec2 spacecraftDir = glm::normalize(spacecraft->getPos() - parentPlanet()->getPos());

    float spacecraftAngle = adjustedAngle(angleBetweenVectors({1.0f, 0.0f}, spacecraftDir));
    float lowerBound = adjustedAngle(absAngle(m_platforms[touchdownPlatformId].angle));
    float touchdownAngle = spacecraftAngle >= lowerBound ?
            spacecraftAngle - lowerBound :
            spacecraftAngle - (lowerBound - TWO_PI);
    float axisAngle = adjustedAngle(touchdownAngle + m_platforms[touchdownPlatformId].angle);

    for (auto &iChunk : chunks()) {
        float chunkDir = adjustedAngle(iChunk.roll());
        float delta = std::abs(axisAngle - chunkDir);

        if (delta > PI)
            delta = TWO_PI - delta;

        float &velocity = iChunk.chunk().velocity;

        constexpr float a = 0.3f;
        constexpr float b = -0.25f;
        constexpr float c = 15.0f;
        constexpr float d = -5.0f;

        velocity = delta * a + b * sinf(c * delta);

        if (axisAngle >= (chunkDir - 3 * ChunkAngle) && axisAngle <= (chunkDir + 3 * ChunkAngle)) {
            velocity = delta * d + b * sinf(c * delta);
        }
    }
}

glm::mat4 PlanetOrbit::falling(int platformID, int chunkID) {
    auto scene = parentGameScene();

    Chunk &chunk = m_platforms[platformID].chunks[chunkID];
    float &radius = chunk.radius;

    float roll = getChunkRoll(platformID, chunkID);

    if (radius > finalPlatformRadius) {
        float &velocity = chunk.velocity;

        const float acceleration = -3.0f;
        auto stepInterval = scene->getScaledFrameTimeSec();

        radius += velocity * stepInterval;
        velocity += acceleration * stepInterval;
    } else {
        if (!chunk.isFell) {
            chunk.isFell = true;
            radius = finalPlatformRadius;
            ++m_fallenChunks;

            if (m_fallenChunks == getChunksCount()) {
                setState(State::OnPlanet);
            }

            // in order to spawn a crystal particle
            // under the planet's surface
            constexpr float radiusMultiplier = 0.99f;

            float absRoll = absAngle(roll);
            auto planet = parentPlanet();

            const glm::vec3 spawnDir {std::cos(absRoll), std::sin(absRoll), 0.0f};
            const glm::vec3 spawnPos = planet->getPos() + (planet->getRadius() * radiusMultiplier) * spawnDir;

            scene->getCrystalParticles().spawn(spawnPos, spawnDir, 1);
        }
    }

    // defaultPlatformRadius is always const because it is radius of the cylinder from blender; chunks are adjusted to it
    // matrix order at getChunkTransform() is very important
    // in deltaScale we scale only a "y" axes of the chunk to adjust it to the new radius

    float scaleFactor = radius / defaultPlatformRadius;

    return chunkTransformMat(radius, roll, scaleFactor);
}

glm::mat4 PlanetOrbit::onPlanet(int platformID, int chunkID) {
    float roll = getChunkRoll(platformID, chunkID);
    float scaleFactor = finalPlatformRadius / defaultPlatformRadius;
    return chunkTransformMat(finalPlatformRadius, roll, scaleFactor);
}

PlanetOrbit::State PlanetOrbit::getState() const {
    return m_state;
}

float PlanetOrbit::getChunkRoll(int platformId, int chunkId) const {
    return m_platforms[platformId].angle + ChunkAngle * static_cast<float>(chunkId);
}

float PlanetOrbit::getChunkRadius(int platformId, int chunkId) const {
    return initialScale * m_platforms[platformId].chunks[chunkId].radius;
}

int PlanetOrbit::getPlatformsCount() const {
    return static_cast<int>(m_platforms.size());
}

int PlanetOrbit::getChunksCount(int platformId) const {
    return static_cast<int>(m_platforms[platformId].chunks.size());
}

int PlanetOrbit::getChunksCount() const {
    int count = 0;
    for (auto &platform : m_platforms)
        count += static_cast<int>(platform.chunks.size());
    return count;
}

int PlanetOrbit::getFallenChunksCount() const {
    return m_fallenChunks;
}

int PlanetOrbit::platformAt(float angle) const {
    if (angle < 0.0f || angle > TWO_PI)
        angle = adjustedAngle(angle);

    for (int i = 0; i < m_platforms.size(); ++i) {
        float lowerBound = m_platforms[i].angle;
        float upperBound = adjustedAngle(lowerBound + static_cast<float>(m_platforms[i].chunks.size()) * ChunkAngle);

        bool isAt;

        if (lowerBound > upperBound) {
            std::swap(lowerBound, upperBound);
            isAt = !(angle >= lowerBound && angle <= upperBound);
        } else {
            isAt = angle >= lowerBound && angle <= upperBound;
        }

        if (isAt) {
            return i;
        }
    }
    return -1;
}

int PlanetOrbit::platformAtAbs(float absAngle) const {
    return platformAt(absAngle - parentPlanet()->getRoll());
}

float PlanetOrbit::absAngle(float angle) const {
    return angle + getRoll();
}

PlanetOrbit::Chunks PlanetOrbit::chunks() {
    return Chunks(*this);
}

Planet* PlanetOrbit::parentPlanet() const {
    return assert_cast<Planet*>(getParent());
}

float PlanetOrbit::getRadius() const {
    return m_radius;
}
