#ifndef ASTROVENTURE_SPACECRAFT_H
#define ASTROVENTURE_SPACECRAFT_H

#include "game/base/SpaceModel.h"
#include "game/mechanics/types.h"

using namespace algine;

class Spacecraft: public SpaceModel {
public:
    explicit Spacecraft(int id, Object *parent = defaultParent());

    int getId() const;

    void setVelocity(const mechanics::vec3 &velocity);
    void setVelocity(const mechanics::vec2 &velocity);

    void changeVelocity(const mechanics::vec3 &velocity);
    void changeVelocity(const mechanics::vec2 &velocity);

    const mechanics::vec3& getVelocity() const;

    float getHeight() const;

    /**
     * Attaches the spacecraft to the specified planet, i.e.
     * translates the spacecraft to the surface of the planet
     * and sets it as the parent of the spacecraft.
     * @param planet The planet to attach the spacecraft to.
     */
    void attachTo(Planet *planet);

private:
    int m_id;

    float m_height;

    mechanics::vec3 m_velocity;
};

#endif //ASTROVENTURE_SPACECRAFT_H
