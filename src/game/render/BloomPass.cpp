#include "BloomPass.h"
#include "game/GameScene.h"
#include "game/GameContent.h"

#include <algine/core/shader/ShaderBuilder.h>

BloomPass::BloomPass(GameRenderer *renderer)
    : GameRenderer::RenderPass(renderer),
      m_multiplier(0.1f),
      m_searchProgram(),
      m_searchFb(),
      m_blur(),
      m_texture(),
      m_dimensions(),
      m_blurDimensions()
{
    auto gameContent = renderer->parentGameScene()->parentGameContent();
    gameContent->addOnSizeChangedListener([this](int width, int height) {
        // TODO: m_multiplier: perform calculations for s_base

        m_dimensions = {width, height};

        m_blurDimensions = {
            static_cast<int>((float) m_dimensions.x * m_multiplier),
            static_cast<int>((float) m_dimensions.y * m_multiplier)
        };

        m_searchFb->resizeAttachments(m_blurDimensions.x, m_blurDimensions.y);
        m_blur->resizeOutput(m_blurDimensions.x, m_blurDimensions.y);
    });
}

void BloomPass::setInput(Texture2D *texture) {
    m_texture = texture;
}

void BloomPass::render() {
    Engine::setViewport(m_blurDimensions);

    m_searchProgram->bind();
    m_searchFb->bind();
    m_searchFb->clearColorBuffer();
    m_texture->use(0);

    auto quadRenderer = parentGameRenderer()->getQuadRenderer();
    quadRenderer->getInputLayout()->bind();
    quadRenderer->draw();

    m_blur->makeBlur(m_searchFb->getAttachedTexture2D(Framebuffer::ColorAttachmentZero));

    // restore the default viewport
    Engine::setViewport(m_dimensions);
}

Texture2D* BloomPass::getOutput() {
    return m_blur->get();
}

LoaderConfig BloomPass::resourceLoaderConfig() {
    return LoaderConfig::create<BloomPass>({
        .loader = [this] { loadResources(); },
        .useContext = true
    });
}

void BloomPass::loadResources() {
    m_searchProgram = new ShaderProgram(this);

    ShaderBuilder fragment(Shader::Type::Fragment);
    fragment.setPath("shaders/BloomSearch.fs.glsl");
    fragment.setParent(m_searchProgram);

    m_searchProgram->attachShader(*QuadRenderer::getVertexShader());
    m_searchProgram->attachShader(*fragment.create());
    m_searchProgram->link();

    m_searchProgram->bind();
    m_searchProgram->loadActiveLocations();
    m_searchProgram->setFloat("brightnessThreshold", 0.6f);
    m_searchProgram->setInt("image", 0);

    auto searchTexture = new Texture2D(this);
    searchTexture->setFormat(Texture::RGB16F);
    searchTexture->bind();
    searchTexture->setParams(Texture2D::defaultParams());
    searchTexture->unbind();

    m_searchFb = new Framebuffer(this);
    m_searchFb->bind();
    m_searchFb->getActiveOutputList().addColor(0);
    m_searchFb->update();
    m_searchFb->attachTexture(searchTexture, Framebuffer::ColorAttachmentZero);
    m_searchFb->unbind();

    TextureCreateInfo createInfo;
    createInfo.format = Texture::RGB16F;
    createInfo.params = Texture2D::defaultParams();

    m_blur = new Blur({
        .textureCreateInfo = std::move(createInfo),
        .kernelRadius = 5,
        .kernelSigma = 16,
        .blurComponents = "rgb"
    }, this);
    m_blur->setQuadRenderer(parentGameRenderer()->getQuadRenderer());
}
