#ifndef SPACE_EXPLORER_PLANETGENERATOR_H
#define SPACE_EXPLORER_PLANETGENERATOR_H

#include "Planet.h"
#include "PlanetGraph.h"
#include "IcoSphere.h"
#include "loader/Loadable.h"

#include <algine/core/math/PCHIPInterpolator.h>
#include <algine/std/model/Mesh.h>

#include <tulz/threading/ThreadPool.h>

using namespace tulz;

class PlanetGenerator: public Object, public Loadable {
public:
    using Callback = std::function<void(Planet*)>;

    constexpr static float crystalsScale = 0.975f;

    constexpr static glm::ivec2 colorMapSize {2, 1};
    constexpr static glm::ivec2 planetColorUV {0, 0};
    constexpr static glm::ivec2 crystalsColorUV {1, 0};

public:
    explicit PlanetGenerator(Object *parent = defaultParent());
    ~PlanetGenerator() override;

    Planet* generate(Planet *base = nullptr);

    /**
     * Generates new planet asynchronously.
     * Callback will be executed in the main UI thread.
     * @param callback
     * @param base
     */
    void generateAsync(Callback callback, Planet *base = nullptr);

    LoaderConfig resourceLoaderConfig() override;

private:
    struct PlanetData {
        std::vector<float> vertices;
        std::vector<float> texCoords;
        std::vector<float> normals;
        std::vector<Mesh> meshes;
        Planet::CrystalsData crystals;
        Planet *base;
    };

private:
    // TODO: move to PlanetManager
    static float calculateAngularVelocity();

    PlanetData cpuStep(Planet *base);
    Planet* gpuStep(PlanetData planetData);

private:
    void generateCrystals(PlanetData &data);

private:
    ThreadPool m_threadPool;
    std::mutex m_mutex;
    IcoSphere m_crystalsBase;
    PlanetGraph::NeighborList m_crystalsGraph;
    PCHIPInterpolator m_crystalsInterpolator;
};

#endif //SPACE_EXPLORER_PLANETGENERATOR_H
