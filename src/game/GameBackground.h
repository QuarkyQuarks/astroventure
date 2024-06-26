#ifndef ASTROVENTURE_GAMEBACKGROUND_H
#define ASTROVENTURE_GAMEBACKGROUND_H

#include <algine/core/buffers/ArrayBuffer.h>
#include <algine/core/InputLayout.h>
#include <algine/core/shader/ShaderProgram.h>
#include <algine/std/ColorMap.h>

#include <tulz/Array.h>
#include <glm/mat4x4.hpp>

#include <vector>

#include "loader/Loadable.h"

using namespace algine;

class Cameraman;
class GameScene;

class GameBackground: public Object, public Loadable {
public:
    constexpr static float AmbientStrength = 0.01f;
    constexpr static float DiffuseStrength = 0.03f;
    constexpr static float SpecularStrength = 0.1f;

    constexpr static float VirtualZ = 2.1f;

public:
    /**
     * @note must be called from the main UI rendering thread
     */
    explicit GameBackground(GameScene &scene, Object *parent = defaultParent());

    void generate();
    void draw(ShaderProgram *program);
    void animate();

    LoaderConfig resourceLoaderConfig() override;

private:
    ArrayBuffer *m_vertexBuffer;
    ArrayBuffer *m_normalsBuffer;
    ArrayBuffer *m_texCoordsBuffer;

    InputLayout *m_layout;

    ColorMap *m_color;

private:
    int m_pointCount;

    std::vector<float> m_vertices;
    std::vector<float> m_normals;
    std::vector<float> m_texCoords;

    std::vector<float> m_zPositions;

    std::vector<std::size_t> m_triangles;

private:
    glm::mat4 m_projection;
    glm::mat4 m_modelView;

    std::vector<float> m_frequency;
    std::vector<float> m_phase;

    GameScene &m_scene;
};

#endif //ASTROVENTURE_GAMEBACKGROUND_H
