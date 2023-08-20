#ifndef ASTROVENTURE_ORBITGENERATOR_H
#define ASTROVENTURE_ORBITGENERATOR_H

#include "PlanetOrbit.h"
#include "loader/Loadable.h"

#include <algine/core/Ptr.h>

using namespace algine;

class OrbitGenerator: public Object, public Loadable {
public:
    explicit OrbitGenerator(Object *parent = defaultParent()); // TODO: PlanetManager as parent

    PlanetOrbit* generate();

    LoaderConfig resourceLoaderConfig() override;

private:
    Shape *m_platformShape;
};


#endif //ASTROVENTURE_ORBITGENERATOR_H
