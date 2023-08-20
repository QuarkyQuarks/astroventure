#include "Spacecraft.h"

Spacecraft::Spacecraft(int id, Object *parent)
    : SpaceModel(SpaceModel::Type::Spacecraft, parent), m_id(id) {}

int Spacecraft::getId() const {
    return m_id;
}