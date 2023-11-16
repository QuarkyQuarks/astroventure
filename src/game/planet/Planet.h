#ifndef SPACE_EXPLORER_PLANET_H
#define SPACE_EXPLORER_PLANET_H

#include <algine/std/ColorMap.h>

#include "game/base/ClusterModel.h"
#include "PlanetGraph.h"

using namespace algine;

class PlanetOrbit;

class Planet: public ClusterModel {
public:
    constexpr static int IndexNone = -1;

    struct CrystalsData {
        std::vector<float> vertices;
        std::vector<float> normals;
        PlanetGraph::List graph;
        int vStart; // crystals vertices start index
    };

public:
    explicit Planet(Object *parent = defaultParent());

    void setIndex(int index);
    int getIndex() const;

    float getRadius() const;
    void setRadius(float radius);

    float getAngularVelocity() const;
    void setAngularVelocity(float angularVelocity);

    void setAtmosphere(bool atmosphere);
    void setAtmosphereRadius(float atmosphereRadius);
    void setAtmosphereColor(const glm::vec3 &atmosphereColor);

    bool hasAtmosphere() const;
    float getAtmosphereRadius() const;
    const glm::vec3& getAtmosphereColor() const;

    void setCrystalsData(CrystalsData data);
    const CrystalsData& getCrystalsData() const;

    void setColorMap(ColorMap *colorMap);
    ColorMap* getColorMap() const;
    Texture2D* getTexture() const;

    PlanetOrbit* getOrbit();

    void update();
    void destroyCrystals();
    void restoreCrystals();

    void setG(float g);

    /**
     * Increments the lock counter.
     * While the planet is locked for reuse, it cannot
     * be used as a base for another planet by the planet manager.
     * @note This function is not thread safe.
     */
    void lockReuse();

    /**
     * Decrements the lock counter.
     * If the counter reaches 0, the planet manager
     * will be allowed to reuse this planet.
     * @note This function is not thread safe.
     */
    void unlockReuse();

    /**
     * Checks whether the planet can be reused or not.
     * @return `true` if the planet can be reused by the planet manager
     * (i.e. the lock counter is equal to 0), `false` otherwise.
     * @note This function is not thread safe.
     */
    bool canBeReused() const;

private:
    // index on the screen (ordinal number)
    int m_index {IndexNone};

    float m_g {};
    float m_radius {0.1f};
    float m_mass {};
    float m_angularVelocity {}; // rad per sec

    bool m_hasAtmosphere {true};
    float m_atmosphereRadius {0.11f};
    glm::vec3 m_atmosphereColor {0.35f, 0.33f, 0.69f};

    CrystalsData m_crystals;

    ColorMap *m_colorMap;

private:
    int m_lockCounter;
};

#endif //SPACE_EXPLORER_PLANET_H
