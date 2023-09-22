#include "DisplayPass.h"

#include <algine/core/shader/ShaderBuilder.h>
#include <algine/core/Framebuffer.h>

DisplayPass::DisplayPass(GameRenderer *parent)
    : GameRenderer::RenderPass(parent),
      m_texture(),
      m_program() {}

void DisplayPass::setTexture(Texture2D *texture) {
    m_texture = texture;
}

void DisplayPass::render() {
    Framebuffer::getDefault()->bind();
    Framebuffer::getDefault()->clear(Framebuffer::DepthBuffer);

    m_program->bind();
    m_texture->use(0);

    auto quadRenderer = parentGameRenderer()->getQuadRenderer();
    quadRenderer->getInputLayout()->bind();
    quadRenderer->draw();
}

LoaderConfig DisplayPass::resourceLoaderConfig() {
    return LoaderConfig::create<DisplayPass>({
        .loader = [this] { loadResources(); },
        .useContext = true
    });
}

void DisplayPass::loadResources() {
    m_program = new ShaderProgram(this);

    ShaderBuilder fragmentBuilder(Shader::Type::Fragment);
    fragmentBuilder.setPath("shaders/Display.fs.glsl");
    fragmentBuilder.setParent(m_program);

    m_program->attachShader(*QuadRenderer::getVertexShader());
    m_program->attachShader(*fragmentBuilder.create());
    m_program->link();
    m_program->loadActiveLocations();
}
