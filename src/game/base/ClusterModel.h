#ifndef ASTROVENTURE_CLUSTERMODEL_H
#define ASTROVENTURE_CLUSTERMODEL_H

#include "SpaceModel.h"

class ClusterModel: public SpaceModel {
public:
    void changeClusterPos(const glm::vec3 &delta);
    void changeClusterRoll(float delta);

    using Object::addChild;
    using Object::removeChild;
    using Object::removeChildren;

protected:
    ClusterModel(Type type, Object *parent);

    static void changeChildrenRoll(float delta, glm::vec2 origin, ClusterModel &clusterModel);
};

#endif //ASTROVENTURE_CLUSTERMODEL_H
