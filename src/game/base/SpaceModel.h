#ifndef ASTROVENTURE_SPACEMODEL_H
#define ASTROVENTURE_SPACEMODEL_H

#include <algine/std/model/Model.h>
#include <algine/std/model/InputLayoutShapeLocations.h>

class Planet;
class PlanetOrbit;
class Spacecraft;
class ClusterModel;
class GameScene;

class SpaceModel: public algine::Model {
public:
    enum class Type {
        Unknown,
        Planet,
        PlanetOrbit,
        Spacecraft
    };

public:
    bool isPlanet() const;
    bool isPlanetOrbit() const;
    bool isSpacecraft() const;
    bool isCluster() const;

    Type getType() const;

    Planet* asPlanet();
    PlanetOrbit* asPlanetOrbit();
    Spacecraft* asSpacecraft();
    ClusterModel* asCluster();

    /**
     * @note this function may be expensive
     * @return instance of the parent scene as GameScene*
     */
    GameScene* parentGameScene() const;

    static algine::InputLayoutShapeLocations getLocations();

protected:
    SpaceModel(Type type, Object *parent);

private:
    Type m_type;
};

#endif //ASTROVENTURE_SPACEMODEL_H
