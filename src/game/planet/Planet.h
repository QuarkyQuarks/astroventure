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
    explicit Planet(Object *parent = defaultParent()); // TODO

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
    ColorMap* getColorMap();
    Texture2D* getTexture() const;

    PlanetOrbit* getOrbit();

    void update();
    void destroyCrystals();
    void restoreCrystals();

    void setG(float g);

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
};

#endif //SPACE_EXPLORER_PLANET_H
