#include "CrystalParticleSystem.h"
#include "PlanetGenerator.h"
#include "game/scheme/ColorScheme.h"
#include "game/math/VecTools.h"

#include <algine/core/Window.h>
#include <algine/std/model/ShapeBuilder.h>

#include "effolkronium/random.hpp"
#include "PerlinNoise/PerlinNoise.hpp"

using Random = effolkronium::random_static;

CrystalParticleSystem::CrystalParticleSystem(GameScene *parent)
    : ParticleSystem(parent),
      m_endPoint() {}

void CrystalParticleSystem::setEndPoint(const glm::vec3 &endPoint) {
    m_endPoint = endPoint;
}

struct CrystalParticleData {
    int step = 0;
};

void CrystalParticleSystem::spawn(const glm::vec3 &pos, const glm::vec3 &dir, int count) {
    for (int i = 0; i < count; ++i) {
        // Note: designated initializes will be supported in c++2a
        addParticle({
            .pos = pos,
            .velocity = dir * Random::get(1.8f, 2.5f),
            .rotate = {0.0f, 0.0f, 0.0f},
            .rotationVelocity = glm::vec3(TWO_PI),
            .scale = Random::get(0.003f, 0.006f),
            .shapeId = 0,
            .data = new CrystalParticleData()
        });

        m_onParticleSpawned.notify();
    }
}

glm::vec3 CrystalParticleSystem::getAcceleration(const ParticleSystem::Particle &particle) const {
    auto data = static_cast<CrystalParticleData*>(particle.data);

    if (data->step == 0 && glm::length(particle.velocity) < 10E-2)
        ++data->step;

    switch (data->step) {
        case 0:
            return -particle.velocity * 12.0f;
        case 1: {
            glm::vec3 dir = glm::normalize(m_endPoint - particle.pos);
            return dir * 3.0f;
        }
    }

    return glm::vec3(0.0f);
}

bool CrystalParticleSystem::isRemove(const ParticleSystem::Particle &particle) const {
    return glm::distance(particle.pos, m_endPoint) < 0.05f;
}

void CrystalParticleSystem::removed(const ParticleSystem::Particle &particle) {
    m_onParticleRemoved.notify();
    delete static_cast<CrystalParticleData*>(particle.data);
}

LoaderConfig CrystalParticleSystem::resourceLoaderConfig() {
    return LoaderConfig::create<CrystalParticleSystem>({
        .loader = [this] { loadResources(); },
        .useContext = true
    });
}

void CrystalParticleSystem::loadResources() {
    IcoSphere sphere(1);
    std::vector<float> vertices(sphere.trianglesCount * 3 * 3);
    std::vector<float> normals(sphere.trianglesCount * 3 * 3);
    std::vector<float> texCoords(sphere.trianglesCount * 3 * 2);

    const siv::BasicPerlinNoise<float>::seed_type seed = rand();
    const siv::BasicPerlinNoise<float> perlin {seed};

    auto uv = ColorMap::normalizeUV(PlanetGenerator::crystalsColorUV, PlanetGenerator::colorMapSize) +
            ColorMap::getCenterOffset(PlanetGenerator::colorMapSize);

    int ix = 0; // triangle index

    for (auto [v1, v2, v3] : sphere.triangles) {
        auto applyNoise = [&perlin](glm::vec3 &p) {
            p *= glm::mix(0.5f, 2.0f, perlin.noise3D_01(p.x, p.y, p.z));
        };

        auto setVertex = []<typename T>(std::vector<float> &vertices, int vIndex, const T &vertex) {
            if constexpr (std::is_same_v<T, glm::vec3>) {
                vertices[vIndex * 3 + 0] = vertex.x;
                vertices[vIndex * 3 + 1] = vertex.y;
                vertices[vIndex * 3 + 2] = vertex.z;
            } else {
                vertices[vIndex * 2 + 0] = vertex.x;
                vertices[vIndex * 2 + 1] = vertex.y;
            }
        };

        glm::vec3 p1 = sphere.getVertex(v1);
        glm::vec3 p2 = sphere.getVertex(v2);
        glm::vec3 p3 = sphere.getVertex(v3);

        applyNoise(p1);
        applyNoise(p2);
        applyNoise(p3);

        auto [n, vertStatus] = getNormal(p1, p2, p3);

        int vb1 = ix * 3 + 0;
        int vb2 = ix * 3 + 1;
        int vb3 = ix * 3 + 2;

        if (vertStatus == -1)
            std::swap(vb1, vb3);

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

    ShapeBuilder shapeBuilder;
    shapeBuilder.setParent(this);
    shapeBuilder.setVertices({std::move(vertices), Buffer::StaticDraw});
    shapeBuilder.setNormals({std::move(normals), Buffer::StaticDraw});
    shapeBuilder.setTexCoords({std::move(texCoords), Buffer::StaticDraw});
    shapeBuilder.loadShape();

    auto shape = shapeBuilder.getCurrentShape();

    Mesh mesh;
    mesh.material.setFloats({
        {Material::AmbientStrength, 0.1f},
        {Material::DiffuseStrength, 0.1f},
        {Material::SpecularStrength, 1.75f},
        {Material::Shininess, 2.5f}
    });
    mesh.start = 0;
    mesh.count = ix * 3;

    shape->setMeshes({mesh});

    std::atomic_bool isInputLayoutCreated {false};

    getParentWindow()->invokeLater([shape, &isInputLayoutCreated, this] {
        shape->createInputLayout(SpaceModel::getLocations());
        m_pool = {shape};
        isInputLayoutCreated = true;
        isInputLayoutCreated.notify_one();
    });

    isInputLayoutCreated.wait(false);
}
