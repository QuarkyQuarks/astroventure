#ifndef ASTROVENTURE_SPACECRAFT_H
#define ASTROVENTURE_SPACECRAFT_H

#include "game/base/SpaceModel.h"
#include "game/mechanics/types.h"

using namespace algine;

class Spacecraft: public SpaceModel {
public:
    explicit Spacecraft(int id, Object *parent = defaultParent()); // TODO

    int getId() const;

    void setVelocity(const mechanics::vec3 &velocity);
    void setVelocity(const mechanics::vec2 &velocity);

    void changeVelocity(const mechanics::vec3 &velocity);
    void changeVelocity(const mechanics::vec2 &velocity);

    const mechanics::vec3& getVelocity() const;

    float getHeight() const;

private:
    int m_id;

    float m_height;

    mechanics::vec3 m_velocity;
};

#endif //ASTROVENTURE_SPACECRAFT_H
