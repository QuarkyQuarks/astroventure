#include "Planet.h"
#include "PlanetGenerator.h"
#include "PlanetOrbit.h"
#include "game/math/VecTools.h"
#include "game/GameScene.h"

#include <algine/core/Engine.h>
#include <algine/core/buffers/ArrayBuffer.h>
#include <algine/std/model/Shape.h>

#include <algorithm>
#include <glm/trigonometric.hpp>

Planet::Planet(Object *parent)
    : ClusterModel(SpaceModel::Type::Planet, parent),
      m_lockCounter(0)
{
    m_colorMap = new ColorMap(this);
}

void Planet::setIndex(int index) {
    m_index = index;
}

int Planet::getIndex() const {
    return m_index;
}

float Planet::getRadius() const {
    return m_radius;
}

void Planet::setRadius(float inRadius) {
    m_radius = inRadius;
}

void Planet::setG(float g) {
    m_g = g;
}

float Planet::getAngularVelocity() const {
    return m_angularVelocity;
}

void Planet::setAngularVelocity(float inAngularVelocity) {
    m_angularVelocity = inAngularVelocity;
}

void Planet::setAtmosphere(bool atmosphere) {
    m_hasAtmosphere = atmosphere;
}

void Planet::setAtmosphereRadius(float atmosphereRadius) {
    m_atmosphereRadius = atmosphereRadius;
}

void Planet::setAtmosphereColor(const glm::vec3 &atmosphereColor) {
    m_atmosphereColor = atmosphereColor;
}

bool Planet::hasAtmosphere() const {
    return m_hasAtmosphere;
}

float Planet::getAtmosphereRadius() const {
    return m_atmosphereRadius;
}

const glm::vec3& Planet::getAtmosphereColor() const {
    return m_atmosphereColor;
}

void Planet::setCrystalsData(CrystalsData data) {
    m_crystals = std::move(data);
}

const Planet::CrystalsData& Planet::getCrystalsData() const {
    return m_crystals;
}

void Planet::setColorMap(ColorMap *colorMap) {
    colorMap->setParent(this);
    if (m_colorMap != nullptr && m_colorMap->getParent() == this)
        delete m_colorMap;
    m_colorMap = colorMap;
}

ColorMap* Planet::getColorMap() const {
    return m_colorMap;
}

Texture2D* Planet::getTexture() const {
    return m_colorMap->get();
}

PlanetOrbit* Planet::getOrbit() {
    return findChild<PlanetOrbit*>(Object::FindOption::Direct);
}

void Planet::update() {
    float deltaRoll = parentGameScene()->getScaledFrameTimeSec() * m_angularVelocity;
    changeClusterRoll(deltaRoll);

    auto orbit = getOrbit();

    if (orbit && orbit->getState() == PlanetOrbit::State::Falling) {
        destroyCrystals();
    }
}

inline void updateBuffer(ArrayBuffer *buffer, uint offset, const std::vector<float> &data) {
    buffer->bind();
    buffer->updateData(offset, data.size() * sizeof(float), data.data());
}

void Planet::destroyCrystals() {
    constexpr float threshold = 0.6f;

    auto shape = getShape();
    auto orbit = getOrbit();

    // TODO: should we skip normals regeneration?
    std::vector<float> vertices(m_crystals.vertices.size());

    auto getVertex = [this, &vertices](int v) -> glm::vec3& {
        memcpy(&vertices[v * 3 + 0], &m_crystals.vertices[v * 3 + 0], 3 * sizeof(float));
        return *reinterpret_cast<glm::vec3*>(&vertices[v * 3 + 0]);
    };

    for (int i = 0; i < vertices.size() / 3; ++i) {
        glm::vec3 &vertex = getVertex(i);
        float vRoll = angleBetweenVectors({1.0f, 0.0f}, glm::vec2(vertex));

        int platformId = orbit->platformAt(vRoll);

        if (platformId == -1)
            continue;

        float minDeltaAngle = INFINITY;
        PlanetOrbit::Chunk *chunk = nullptr;

        PlanetOrbit::Chunks::Iterator chBegin({platformId, 0, orbit});
        PlanetOrbit::Chunks::Iterator chEnd({platformId + 1, 0, orbit});

        // find the closest chunk
        for (auto it = chBegin; it != chEnd; ++it) {
            auto &inf = *it;

            if (float deltaAngle = std::abs(inf.roll() - vRoll); deltaAngle < minDeltaAngle) {
                minDeltaAngle = deltaAngle;
                chunk = &inf.chunk();
            }
        }

        float k = (chunk->radius - PlanetOrbit::finalPlatformRadius) / (PlanetOrbit::defaultPlatformRadius - PlanetOrbit::finalPlatformRadius);
        float scaleFactor = glm::mix(0.7f, PlanetGenerator::crystalsScale, std::min(k, 1.0f));

        if (abs(vertex.z) < threshold) {
            vertex *= scaleFactor;
        }
    }

    updateBuffer(shape->getVerticesBuffer(), m_crystals.vStart * 3 * sizeof(float), vertices);
}

void Planet::restoreCrystals() {
    auto shape = getShape();
    updateBuffer(shape->getVerticesBuffer(), m_crystals.vStart * 3 * sizeof(float), m_crystals.vertices);
}

void Planet::lockReuse() {
    ++m_lockCounter;
}

void Planet::unlockReuse() {
    if (m_lockCounter == 0)
        throw std::runtime_error("The number of unlocks is greater than the number of locks");
    --m_lockCounter;
}

bool Planet::canBeReused() const {
    return m_lockCounter == 0;
}
