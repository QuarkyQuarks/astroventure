#include "SpaceModel.h"
#include "ClusterModel.h"
#include "game/planet/Planet.h"
#include "game/planet/PlanetOrbit.h"
#include "game/spacecraft/Spacecraft.h"
#include "game/GameScene.h"

#include <algine/core/assert_cast.h>

bool SpaceModel::isPlanet() const {
    return m_type == Type::Planet;
}

bool SpaceModel::isPlanetOrbit() const {
    return m_type == Type::PlanetOrbit;
}

bool SpaceModel::isSpacecraft() const {
    return m_type == Type::Spacecraft;
}

bool SpaceModel::isCluster() const {
    return dynamic_cast<const ClusterModel*>(this) != nullptr;
}

SpaceModel::Type SpaceModel::getType() const {
    return m_type;
}

Planet* SpaceModel::asPlanet() {
    return as<Planet*>();
}

PlanetOrbit* SpaceModel::asPlanetOrbit() {
    return as<PlanetOrbit*>();
}

Spacecraft* SpaceModel::asSpacecraft() {
    return as<Spacecraft*>();
}

ClusterModel* SpaceModel::asCluster() {
    return as<ClusterModel*>();
}

GameScene* SpaceModel::parentGameScene() const {
    return assert_cast<GameScene*>(getParentScene());
}

SpaceModel::SpaceModel(SpaceModel::Type type, Object *parent)
    : algine::Model(parent), m_type(type) {}

InputLayoutShapeLocations SpaceModel::getLocations() {
    InputLayoutShapeLocations locations;
    locations["position"] = 0;
    locations["normal"] = 1;
    locations["texCoord"] = 2;
    return locations;
}
