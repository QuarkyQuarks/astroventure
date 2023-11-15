#include "GameBackground.h"

#include "math/VecTools.h"
#include "game/scheme/ColorScheme.h"
#include "game/GameScene.h"
#include "game/GameContent.h"

#include <algine/core/Engine.h>

#include "effolkronium/random.hpp"
#include "delaunator.hpp"
#include "poisson_disk_sampling.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

using Random = effolkronium::random_static;
using namespace delaunator;
using namespace thinks;
using namespace tulz;

GameBackground::GameBackground(GameScene &scene, Object *parent)
    : Object(parent),
      m_vertexBuffer(new ArrayBuffer(this)),
      m_normalsBuffer(new ArrayBuffer(this)),
      m_texCoordsBuffer(new ArrayBuffer(this)),
      m_layout(new InputLayout(this)),
      m_color(new ColorMap(1, 1, Texture::RGB16F, this)), // TODO
      m_pointCount(0),
      m_projection(),
      m_modelView(),
      m_scene(scene)
{
    auto addAttribute = [this](int location, int count, ArrayBuffer *buffer) {
        InputAttributeDescription attribDescription;
        attribDescription.setLocation(location);
        attribDescription.setCount(count);
        attribDescription.setDataType(DataType::Float);
        attribDescription.setStride(count * sizeof(float));
        attribDescription.setOffset(0);

        m_layout->bind();
        m_layout->addAttribute(attribDescription, buffer);
        m_layout->unbind();
    };

    addAttribute(0, 3, m_vertexBuffer);
    addAttribute(1, 3, m_normalsBuffer);
    addAttribute(2, 2, m_texCoordsBuffer);

    scene.parentGameContent()->addOnSizeChangedListener([this](int width, int height) {
        m_projection = glm::perspective(PI / 6, (float) width / (float) height, 1.0f, 64.0f);
    });

    auto updateColor = [this] {
        m_color->setColor({0, 0}, m_scene.getColorSchemeManager().getColorScheme().gameBackground);
        m_color->update();
    };

    updateColor();

    scene.getColorSchemeManager().addOnChangedListener(updateColor);
}

void GameBackground::generate() {
    auto setBuffer = [](ArrayBuffer *buffer, const std::vector<float> &data) {
        buffer->bind();
        buffer->setData(data.size() * sizeof(float), data.data(), Buffer::DynamicDraw);
    };

    constexpr auto radius = 0.15f;
    constexpr auto minBorder = std::array<float, 2>{{-1.7f, -3.2f}};
    constexpr auto maxBorder = std::array<float, 2>{{1.7f, 3.2f}};

    auto sampledPoints = PoissonDiskSampling(radius, minBorder, maxBorder, 30, Random::get(0, 100));

    m_pointCount = static_cast<int>(sampledPoints.size());

    std::vector<double> points;
    points.reserve(m_pointCount * 2);

    for (auto &point : sampledPoints) {
        points.emplace_back(point[0]);
        points.emplace_back(point[1]);
    }

    Delaunator triangulatedPoints(points);

    std::swap(m_triangles, triangulatedPoints.triangles);

    // init z delta coordinates buffer
    m_zPositions.resize(m_pointCount);

    for (uint i = 0; i < m_pointCount; ++i)
        m_zPositions[i] = Random::get(-9.0f, -8.0f);

    m_vertices.reserve(m_triangles.size() * 3);

    for (int i = 0; i < m_triangles.size(); i += 3) {
        auto addVertex = [&](int v) {
            m_vertices.emplace_back(triangulatedPoints.coords[2 * m_triangles[v] + 0]);
            m_vertices.emplace_back(triangulatedPoints.coords[2 * m_triangles[v] + 1]);
            m_vertices.emplace_back(m_zPositions[m_triangles[v]]);
        };

        addVertex(i + 0);
        addVertex(i + 2);
        addVertex(i + 1);
    }

    setBuffer(m_vertexBuffer, m_vertices);

    // normals
    m_normals.resize(m_vertices.size());
    setBuffer(m_normalsBuffer, m_normals);

    // texture coordinates

    const auto texCoordSize = m_vertices.size() / 3 * 2;
    m_texCoords.resize(texCoordSize, 0.0f);

    setBuffer(m_texCoordsBuffer, m_texCoords);

    // init animation arrays
    m_frequency.resize(m_pointCount);
    m_phase.resize(m_pointCount);

    for (uint i = 0; i < m_pointCount; ++i) {
        m_frequency[i] = Random::get(1.25f, 1.75f);
        m_phase[i] = Random::get(-20.0f, 20.0f);
    }
}

void GameBackground::draw(ShaderProgram *program) {
    program->setFloat("ambientStrength", AmbientStrength);
    program->setFloat("diffuseStrength", DiffuseStrength);
    program->setFloat("specularStrength", SpecularStrength);
    program->setMat4("MVPMatrix", m_projection * m_modelView);
    program->setMat4("modelView", m_modelView);

    m_color->get()->use(0);

    m_layout->bind();
    Engine::disableDepthMask();
    Engine::drawArrays(0, m_triangles.size());
    Engine::enableDepthMask();
}

void GameBackground::animate() {
    auto &camera = m_scene.getCameraman().getCamera();
    auto k = glm::smoothstep(1.0f, Cameraman::GamePos.z, length(camera.getPos()));

    m_modelView = translate(glm::mat4(1.0f), {0.0f, 0.0f, -VirtualZ * k});

    // taking into account time
    auto time = m_scene.getGameTimeSec();

    // vertices & normals

    auto zUpdate = [time, this](uint i) {
        constexpr float amplitude = 0.7f;

        float max_z = m_zPositions[i] + amplitude;
        float min_z = m_zPositions[i] - amplitude;

        float ratio = 0.5f * (1.0f + sinf(m_phase[i] + m_frequency[i] * time));

        return glm::mix(min_z, max_z, ratio);
    };

    for (uint i = 0; i < m_vertices.size(); i += 9) {
        m_vertices[i + 2] = zUpdate(m_triangles[i / 3]);
        m_vertices[i + 5] = zUpdate(m_triangles[i / 3 + 2]);
        m_vertices[i + 8] = zUpdate(m_triangles[i / 3 + 1]);

        // calculate normal

        glm::vec3 a {m_vertices[i + 0], m_vertices[i + 1], m_vertices[i + 2]};
        glm::vec3 b {m_vertices[i + 3], m_vertices[i + 4], m_vertices[i + 5]};
        glm::vec3 c {m_vertices[i + 6], m_vertices[i + 7], m_vertices[i + 8]};

        glm::vec3 vector1 = a - b;
        glm::vec3 vector2 = b - c;

        glm::vec3 crossProduct = glm::normalize(glm::cross(vector1, vector2));

        for (int n = 0; n < 9; n += 3) {
            m_normals[i + n] = crossProduct.x;
            m_normals[i + (1 + n)] = crossProduct.y;
            m_normals[i + (2 + n)] = crossProduct.z;
        }
    }

    auto updateBuffer = [](ArrayBuffer *buffer, const std::vector<float> &data) {
        buffer->bind();
        buffer->updateData(0, data.size() * sizeof(float), data.data());
    };

    updateBuffer(m_vertexBuffer, m_vertices);
    updateBuffer(m_normalsBuffer, m_normals);
}

LoaderConfig GameBackground::resourceLoaderConfig() {
    return LoaderConfig::create<GameBackground>({
        .loader = [this] { generate(); },
        .useContext = true
    });
}