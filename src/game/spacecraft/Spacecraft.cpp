#include "Spacecraft.h"
#include "game/planet/Planet.h"

Spacecraft::Spacecraft(int id, Object *parent)
    : SpaceModel(SpaceModel::Type::Spacecraft, parent),
      m_id(id),
      m_height(0.04),
      m_velocity() {}

int Spacecraft::getId() const {
    return m_id;
}

void Spacecraft::setVelocity(const mechanics::vec3 &velocity) {
    m_velocity = velocity;
}

void Spacecraft::setVelocity(const mechanics::vec2 &velocity) {
    m_velocity = {velocity.x, velocity.y, m_velocity.z};
}

void Spacecraft::changeVelocity(const mechanics::vec3 &delta) {
    m_velocity += delta;
}

void Spacecraft::changeVelocity(const mechanics::vec2 &delta) {
    m_velocity += (mechanics::vec3) {delta.x, delta.y, 0};
}

const mechanics::vec3 & Spacecraft::getVelocity() const {
    return m_velocity;
}

float Spacecraft::getHeight() const {
    return m_height;
}

void Spacecraft::attachTo(Planet *planet) {
    setX(planet->getX());
    setY(planet->getY() + planet->getRadius());
    setRoll(0);
    setParent(planet);
}