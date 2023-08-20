#include "AtmosphereRenderer.h"

#include <algine/core/shader/ShaderBuilder.h>
#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/PtrMaker.h>
#include <algine/core/Engine.h>

#include <glm/gtc/matrix_transform.hpp>

AtmosphereRenderer::AtmosphereRenderer(Camera &camera, Object *parent)
    : Object(parent),
      m_camera(camera),
      m_layout(new InputLayout(this)),
      m_vertices(new ArrayBuffer(this)),
      m_program()
{
    auto addAttribute = [this](int location, int count, ArrayBuffer *buffer) {
        InputAttributeDescription attribDescription;
        attribDescription.setLocation(location);
        attribDescription.setCount(count);
        attribDescription.setDataType(DataType::Float);

        m_layout->bind();
        m_layout->addAttribute(attribDescription, buffer);
        m_layout->unbind();
    };

    const float vertices[] = {
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };

    m_vertices->bind();
    m_vertices->setData(sizeof(vertices), vertices, Buffer::StaticDraw);

    addAttribute(0, 3, m_vertices);
}

void AtmosphereRenderer::begin() {
    m_layout->bind();
    m_program->bind();
}

void AtmosphereRenderer::render(Planet *planet) {
    glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(planet->getAtmosphereRadius()));
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), planet->getPos());
    glm::mat4 model = translation * scaling * glm::inverse(m_camera.getRotationMatrix());

    m_program->setVec3("color", planet->getAtmosphereColor());
    m_program->setMat4("mvp", m_camera.getProjectionMatrix() * m_camera.getViewMatrix() * model);

    Engine::drawArrays(0, 4, Engine::PolyType::TriangleStrip);
}

LoaderConfig AtmosphereRenderer::resourceLoaderConfig() {
    return LoaderConfig::create<AtmosphereRenderer>({
        .loader = [this] { loadResources(); },
        .useContext = true
    });
}

void AtmosphereRenderer::loadResources() {
    m_program = new ShaderProgram(this);

    ShaderBuilder vertexBuilder(Shader::Type::Vertex);
    vertexBuilder.setPath("shaders/Atmosphere.vs.glsl");
    vertexBuilder.setParent(m_program);

    ShaderBuilder fragmentBuilder(Shader::Type::Fragment);
    fragmentBuilder.setPath("shaders/Atmosphere.fs.glsl");
    fragmentBuilder.setParent(m_program);

    m_program->attachShader(*vertexBuilder.create());
    m_program->attachShader(*fragmentBuilder.create());
    m_program->link();
    m_program->loadActiveLocations();
}
