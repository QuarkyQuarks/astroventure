#include "PlanetGenerator.h"

#include "game/math/MathConstants.h"
#include "game/math/VecTools.h"
#include "game/GameScene.h"

#include <algine/core/Window.h>
#include <algine/core/log/Log.h>
#include <algine/std/model/ShapeBuilder.h>

#include "effolkronium/random.hpp"

#include "PointCloudCrust/PointCloudCrust.h"
#include "PerlinNoise/PerlinNoise.hpp"
#include "poisson_disk_sampling.h"

#include <thread>
#include <array>

using namespace tulz;
using namespace thinks;
using namespace congard::PointCloudCrust;

using Random = effolkronium::random_static;

PlanetGenerator::PlanetGenerator(Object *parent)
    : Object(parent)
{
    setThreadSafety(true);

    // TODO: perform testing; possibly it is too much, maybe should be multiplied by 0.75
    m_threadPool.setMaxThreadCount(std::max((int) std::thread::hardware_concurrency(), 2));
    m_threadPool.setExpiryTimeout(60000);
}

PlanetGenerator::~PlanetGenerator() {
    m_threadPool.stop();
}

Planet* PlanetGenerator::generate(Planet *base) {
    return gpuStep(cpuStep(base));
}

void PlanetGenerator::generateAsync(Callback callback, Planet *base) {
    // AHTUNG!
    // Code below a little bit complicated because of threading.
    // We must take into account variables lifecycle:
    // thread will not start immediately, so local variables may already be destroyed.
    // We use std::move to prevent from extra copying

    m_threadPool.start([this, postAction = std::move(callback), base]() mutable {
        auto data = cpuStep(base);

        getParentWindow()->invokeLater([this, data = std::move(data), postAction = std::move(postAction)]() {
            InputLayout::getDefault()->bind();
            auto planet = gpuStep(data);
            postAction(planet);
            return true;
        });
    });
}

float PlanetGenerator::calculateAngularVelocity() {
    auto angularVelocityDir = static_cast<float>(Random::get<bool>() * 2 - 1); // -1 or 1
    auto angularVelocity = angularVelocityDir * PI / 2.3f;
    return angularVelocity;
}

template<typename T>
void setVertex(std::vector<float> &vertices, int vIndex, const T &vertex) {
    if constexpr (std::is_same_v<T, glm::vec3>) {
        vertices[vIndex * 3 + 0] = vertex.x;
        vertices[vIndex * 3 + 1] = vertex.y;
        vertices[vIndex * 3 + 2] = vertex.z;
    } else {
        vertices[vIndex * 2 + 0] = vertex.x;
        vertices[vIndex * 2 + 1] = vertex.y;
    }
}

template<typename T>
void emplaceVertex(std::vector<float> &vertices, const T &vertex) {
    if constexpr (std::is_same_v<T, glm::vec3>) {
        vertices.emplace_back(vertex.x);
        vertices.emplace_back(vertex.y);
        vertices.emplace_back(vertex.z);
    } else {
        vertices.emplace_back(vertex.x);
        vertices.emplace_back(vertex.y);
    }
}

// generate planet
PlanetGenerator::PlanetData PlanetGenerator::cpuStep(Planet *base) {
    // generate sphere vertices
    constexpr auto radius = 0.15f; // TODO: maybe a little bit randomize?
    constexpr auto minBorder = std::array<float, 3>{{-1.0f, -1.0f, -1.0f}};
    constexpr auto maxBorder = std::array<float, 3>{{1.0f, 1.0f, 1.0f}};

    auto poissonPoints = PoissonDiskSampling(radius, minBorder, maxBorder, 30, rand()); // TODO: use better random
    std::vector<float> planetVertices;

    for (auto &point : poissonPoints) {
        float dist = sqrtf(powf(point[0], 2) + powf(point[1], 2) + powf(point[2], 2));

        if (dist >= 0.85f && dist <= 1.0f) {
            glm::vec3 vert = glm::normalize(glm::vec3(point[0], point[1], point[2]));
            planetVertices.emplace_back(vert.x);
            planetVertices.emplace_back(vert.y);
            planetVertices.emplace_back(vert.z);
        }
    }

    // construct sphere surface
    PointCloudCrust pointCloudCrust;
    pointCloudCrust.setRadius(0.4f); // ≈ radius * 2 + d
    pointCloudCrust.setPoints(planetVertices.data(), planetVertices.size(), false, false);

    // multithreaded mode

    {
        auto n = m_threadPool.getMaxThreadCount();
        int processed = 0;

        std::mutex m;
        std::condition_variable cv;

        for (int i = 0; i < n; ++i) {
            m_threadPool.start([&](int i) {
                pointCloudCrust.computeRange((float) i / n, (float) (i + 1) / n);

                std::unique_lock lk(m);
                ++processed;
                lk.unlock();
                cv.notify_one();

                Log::info("PlanetGenerator", std::to_string(i) + " done");
            }, static_cast<int>(i)); // casting is for copy
        }

        std::unique_lock lk(m);
        cv.wait(lk, [&] { return processed == n; });
        Log::info("PlanetGenerator", "Done: " + std::to_string(processed));
    }

    // pointCloudCrust.optimize(); // not needed

    auto &pccTriangles = pointCloudCrust.getTriangles();
    auto trianglesSize = std::distance(pccTriangles.begin(), pccTriangles.end());

    auto &cloudVertices = pointCloudCrust.getPoints();

    // make buffers

    std::vector<float> vertices(trianglesSize * 3 * 3); // 3x(xyz)
    std::vector<float> normals(trianglesSize * 3 * 3);
    std::vector<float> texCoords(trianglesSize * 3 * 2); // 3x(uv)

    auto uv = ColorMap::normalizeUV(planetColorUV, colorMapSize) + ColorMap::getCenterOffset(colorMapSize);

    int ix = 0; // triangle index

    for (auto [v1, v2, v3] : pccTriangles) {
        int vb1 = ix * 3 + 0;
        int vb2 = ix * 3 + 1;
        int vb3 = ix * 3 + 2;

        // we can do such cast because glm::vec3 and Vector have the same memory layout
        auto &p1 = *reinterpret_cast<glm::vec3*>(&cloudVertices.getPoint(v1));
        auto &p2 = *reinterpret_cast<glm::vec3*>(&cloudVertices.getPoint(v2));
        auto &p3 = *reinterpret_cast<glm::vec3*>(&cloudVertices.getPoint(v3));

        auto [n, vertStatus] = getNormal(p1, p2, p3);

        // fix vertex order if needed
        if (vertStatus == -1) {
            // std::swap(p1, p3);
            std::swap(vb1, vb3);
        }

        setVertex(vertices, vb1, p1);
        setVertex(vertices, vb2, p2);
        setVertex(vertices, vb3, p3);

        setVertex(normals, vb1, n);
        setVertex(normals, vb2, n);
        setVertex(normals, vb3, n);

        setVertex(texCoords, vb1, uv);
        setVertex(texCoords, vb2, uv);
        setVertex(texCoords, vb3, uv);

        ++ix;
    }

    std::vector<Mesh> planetMeshes;
    Mesh mesh;
    mesh.material.setFloats({
        {Material::AmbientStrength, 0.3f},
        {Material::DiffuseStrength, 0.13f},
        {Material::SpecularStrength, 0.18f},
        {Material::Shininess, 2.3f}
    });
    mesh.start = 0;
    mesh.count = trianglesSize * 3;
    planetMeshes.emplace_back(mesh);

    PlanetData data;
    data.vertices = std::move(vertices);
    data.texCoords = std::move(texCoords);
    data.normals = std::move(normals);
    data.meshes = std::move(planetMeshes);
    data.base = base;

    generateCrystals(data);

    return std::move(data);
}

void PlanetGenerator::generateCrystals(PlanetData &data) {
    std::vector<float> crystalVertices = m_crystalsBase.vertices;
    auto vCount = static_cast<int>(m_crystalsBase.vertices.size() / 3);

    auto getCrystalVertex = [&crystalVertices](int v) -> glm::vec3& {
        return *reinterpret_cast<glm::vec3*>(&crystalVertices[v * 3 + 0]);
    };

    const siv::BasicPerlinNoise<float>::seed_type seed = rand();
    const siv::BasicPerlinNoise<float> perlin{seed};

    // apply noise to the crystals vertices

    for (int i = 0; i < vCount; ++i) {
        auto &v = getCrystalVertex(i);
        float noise = perlin.octave3D_01(v.x, v.y, v.z, 4);
        v *= glm::mix(1.0f, 1.2f, m_crystalsInterpolator.getY(noise));
    }

    // optimize mesh: remove all vertices which length is 1 and which
    // are connected with vertices with the same length (i.e. 1)

    // negative value means that vertex marked as removed
    std::vector<int> vertexOffsets(vCount, 0);
    int offset = 0;

    for (int v = 0; v < vCount; ++v) {
        // all vertices in the vertex array have length >= 1
        auto isLenLessThanOne = [](const glm::vec3 &vec) {
            constexpr float epsilon = 10E-8;
            return glm::length(vec) < 1 + epsilon;
        };

        if (isLenLessThanOne(getCrystalVertex(v))) {
            // check neighbors
            bool remove = true;

            for (int u : m_crystalsGraph[v]) {
                if (!isLenLessThanOne(getCrystalVertex(u))) {
                    remove = false;
                    break;
                }
            }

            if (remove) {
                vertexOffsets[v] = -1;
                ++offset;
                continue;
            }
        }

        vertexOffsets[v] = offset;
    }

    PlanetGraph graph(vCount - offset);
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    int ix = 0; // triangle index; counter

    // reserve memory as much as possible since we
    // don't know the number of removed triangles
    vertices.reserve(m_crystalsBase.trianglesCount * 3 * 3);
    normals.reserve(m_crystalsBase.trianglesCount * 3 * 3);
    texCoords.reserve(m_crystalsBase.trianglesCount * 3 * 2);

    auto uv = ColorMap::normalizeUV(crystalsColorUV, colorMapSize) + ColorMap::getCenterOffset(colorMapSize);

    for (auto [v1, v2, v3] : m_crystalsBase.triangles) {
        if (vertexOffsets[v1] == -1 || vertexOffsets[v2] == -1 || vertexOffsets[v3] == -1)
            continue;

        // relative vertex buffer indices
        int vb1 = ix * 3 + 0;
        int vb2 = ix * 3 + 1;
        int vb3 = ix * 3 + 2;

        auto p1 = getCrystalVertex(v1) * crystalsScale;
        auto p2 = getCrystalVertex(v2) * crystalsScale;
        auto p3 = getCrystalVertex(v3) * crystalsScale;

        auto [n, vertStatus] = getNormal(p1, p2, p3);

        // fix vertex order if needed
        if (vertStatus == -1) {
            std::swap(p1, p3);
            std::swap(vb1, vb3);
        }

        emplaceVertex(vertices, p1);
        emplaceVertex(vertices, p2);
        emplaceVertex(vertices, p3);

        emplaceVertex(normals, n);
        emplaceVertex(normals, n);
        emplaceVertex(normals, n);

        emplaceVertex(texCoords, uv);
        emplaceVertex(texCoords, uv);
        emplaceVertex(texCoords, uv);

        // offsetted indices
        int v1o = v1 - vertexOffsets[v1];
        int v2o = v2 - vertexOffsets[v2];
        int v3o = v3 - vertexOffsets[v3];

        graph.edgeTo(v1o, v2o);
        graph.edgeTo(v1o, v3o);
        graph.edgeTo(v2o, v3o);

        graph.associate(v1o, vb1, ix);
        graph.associate(v2o, vb2, ix);
        graph.associate(v3o, vb3, ix);

        ++ix;
    }

    // insert result

    data.vertices.insert(data.vertices.end(), vertices.begin(), vertices.end());
    data.normals.insert(data.normals.end(), normals.begin(), normals.end());
    data.texCoords.insert(data.texCoords.end(), texCoords.begin(), texCoords.end());
    data.crystals.vertices = std::move(vertices);
    data.crystals.normals = std::move(normals);
    data.crystals.graph = graph.list();
    data.crystals.vStart = static_cast<int>(data.vertices.size() / 3 - ix * 3);

    Mesh mesh;
    mesh.material.setFloats({
        {Material::AmbientStrength, 0.2f},
        {Material::DiffuseStrength, 0.13f},
        {Material::SpecularStrength, 0.7f},
        {Material::Shininess, 2.5f}
    });
    mesh.start = data.meshes[0].count;
    mesh.count = ix * 3;
    data.meshes.emplace_back(mesh);
    //data.meshes[0] = mesh;
}

Planet* PlanetGenerator::gpuStep(PlanetData planetData) {
    float angularVelocity = calculateAngularVelocity();

    auto &colorScheme = findParent<GameScene*>()->getColorSchemeManager().getColorScheme();

    auto updateColorMap = [&](ColorMap *colorMap) {
        colorMap->setColor(planetColorUV, colorScheme.planetColor);
        colorMap->setColor(crystalsColorUV, colorScheme.crystalsColor);
        colorMap->update();
    };

    // try to reuse a planet
    if (planetData.base != nullptr) {
        auto planet = planetData.base;
        planet->setParent(this);
        planet->setCrystalsData(std::move(planetData.crystals));
        planet->setAngularVelocity(angularVelocity);
        planet->setRoll(0);

        auto shape = planet->getShape();
        shape->setMeshes(planetData.meshes);

        auto updateBuffer = [](ArrayBuffer *buffer, const std::vector<float> &data) {
            buffer->bind();

            if (buffer->size() >= data.size() * sizeof(float)) {
                buffer->updateData(0, data.size() * sizeof(float), data.data());
            } else {
                buffer->setData(data.size() * sizeof(float), data.data(), Buffer::DynamicDraw);
            }
        };

        updateBuffer(shape->getVerticesBuffer(), planetData.vertices);
        updateBuffer(shape->getNormalsBuffer(), planetData.normals);
        updateBuffer(shape->getTexCoordsBuffer(), planetData.texCoords);

        // update color scheme
        updateColorMap(planet->getColorMap());
        planet->setAtmosphereColor(colorScheme.atmosphereColor.getRgbF());

        return planet;
    }

    // otherwise, create a new one

    auto planet = new Planet(this);

    ShapeBuilder shapeBuilder;
    shapeBuilder.setParent(planet);
    shapeBuilder.setVertices({std::move(planetData.vertices), Buffer::DynamicDraw});
    shapeBuilder.setNormals({std::move(planetData.normals), Buffer::DynamicDraw});
    shapeBuilder.setTexCoords({std::move(planetData.texCoords), Buffer::DynamicDraw});
    shapeBuilder.addInputLayoutLocations(SpaceModel::getLocations());
    shapeBuilder.loadShape();

    auto shape = shapeBuilder.getCurrentShape();
    shape->setMeshes(planetData.meshes);

    // create a color map
    auto colorMap = new ColorMap(colorMapSize, Texture::RGB16F); // TODO
    updateColorMap(colorMap);

    planet->setRotatorType(Rotator::Type::Euler);
    planet->setShape(shape);
    planet->setScale(glm::vec3(0.08f));
    planet->scale();
    planet->setAngularVelocity(angularVelocity);
    planet->setCrystalsData(std::move(planetData.crystals));
    planet->setColorMap(colorMap);
    planet->setAtmosphereColor(colorScheme.atmosphereColor.getRgbF());

    return planet;
}

LoaderConfig PlanetGenerator::resourceLoaderConfig() {
    return LoaderConfig::create<PlanetGenerator>({
        .loader = [this]() {
            m_crystalsInterpolator.setPoints({
                {0.0f, 0.0f},
                {0.5f, 0.0f},
                {0.7f, 0.6f},
                {1.0f, 1.0f}
            });

            m_crystalsBase.gen(3);

            PlanetGraph crystalsGraph(m_crystalsBase.vertices.size());

            for (auto [v1, v2, v3] : m_crystalsBase.triangles) {
                crystalsGraph.edgeTo(v1, v2);
                crystalsGraph.edgeTo(v1, v3);
                crystalsGraph.edgeTo(v2, v3);
            }

            m_crystalsGraph = crystalsGraph.neighborList();
        }
    });
}
