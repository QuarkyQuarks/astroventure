#include "ClusterModel.h"

ClusterModel::ClusterModel(Type type, Object *parent)
    : SpaceModel(type, parent) {}

void ClusterModel::changeClusterPos(const glm::vec3 &delta) {
    changePos(delta);
    translate();

    forEachChild<SpaceModel*>([&](SpaceModel *child) {
        if (auto cluster = child->asCluster(); cluster != nullptr) {
            cluster->changeClusterPos(delta);
        } else {
            child->changePos(delta);
            child->translate();
        }

        return IterationFlag::Continue;
    });
}

void ClusterModel::changeClusterRoll(float delta) {
    setRoll(getRoll() + delta);
    rotate();
    changeChildrenRoll(delta, {getX(), getY()}, *this);
}

void ClusterModel::changeChildrenRoll(float delta, glm::vec2 origin, ClusterModel &clusterModel) {
    // to rotate a child, we need:
    //  1. change its roll
    //  2. rotate it around the origin

    clusterModel.forEachChild<SpaceModel*>([&](SpaceModel *child) {
        child->setRoll(child->getRoll() + delta);
        child->rotate();

        glm::vec2 globalChildPos {child->getX(), child->getY()};
        glm::vec2 childPos = globalChildPos - origin;

        float x = childPos.x * cosf(delta) - childPos.y * sinf(delta);
        float y = childPos.x * sinf(delta) + childPos.y * cosf(delta);

        child->setPos(x + origin.x, y + origin.y, child->getZ());
        child->translate();

        if (auto cluster = child->asCluster(); cluster != nullptr) {
            changeChildrenRoll(delta, origin, *cluster);
        }

        return IterationFlag::Continue;
    });
}
