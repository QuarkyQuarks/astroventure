#include "MergePass.h"
#include "game/GameScene.h"
#include "game/GameContent.h"

#include <algine/core/shader/ShaderBuilder.h>

MergePass::MergePass(GameRenderer *renderer)
    : GameRenderer::RenderPass(renderer),
      m_base(),
      m_bloom(),
      m_framebuffer(new Framebuffer(this)),
      m_program()
{
    auto gameContent = renderer->parentGameScene()->parentGameContent();
    gameContent->addOnSizeChangedListener([this](int width, int height) {
        m_framebuffer->resizeAttachments(width, height);
    });

    auto texture = new Texture2D(m_framebuffer);
    texture->setFormat(Texture::RGB16F); // TODO: rgb8?
    texture->bind();
    texture->setParams(Texture2D::defaultParams());
    texture->unbind();

    m_framebuffer->bind();
    m_framebuffer->getActiveOutputList().addColor(0);
    m_framebuffer->update();
    m_framebuffer->attachTexture(texture, Framebuffer::ColorAttachmentZero);
    m_framebuffer->unbind();
}

void MergePass::setInputs(Texture2D *base, Texture2D *bloom) {
    m_base = base;
    m_bloom = bloom;
}

Texture2D* MergePass::getOutput() {
    return m_framebuffer->getAttachedTexture2D(Framebuffer::ColorAttachmentZero);
}

Framebuffer* MergePass::getOutputFramebuffer() {
    return m_framebuffer;
}

void MergePass::render() {
    m_framebuffer->bind();
    m_program->bind();
    m_base->use(0);
    m_bloom->use(1);

    auto quadRenderer = parentGameRenderer()->getQuadRenderer();
    quadRenderer->getInputLayout()->bind();
    quadRenderer->draw();
}

LoaderConfig MergePass::resourceLoaderConfig() {
    return LoaderConfig::create<MergePass>({
        .loader = [this] { loadResources(); },
        .useContext = true
    });
}

void MergePass::loadResources() {
    m_program = new ShaderProgram(this);

    ShaderBuilder fragment(Shader::Type::Fragment);
    fragment.setPath("shaders/Merge.fs.glsl");
    fragment.setParent(m_program);

    m_program->attachShader(*QuadRenderer::getVertexShader());
    m_program->attachShader(*fragment.create());
    m_program->link();

    m_program->bind();
    m_program->loadActiveLocations();
    m_program->setFloat("gamma", 1.0f);
    m_program->setInt("image", 0);
    m_program->setInt("bloomImage", 1);
}
