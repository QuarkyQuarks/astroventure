#include "GameRenderPass.h"
#include "GameRenderer.h"
#include "game/GameScene.h"
#include "game/GameContent.h"

#include <algine/core/shader/ShaderProgramBuilder.h>
#include <algine/core/Engine.h>

GameRenderPass::GameRenderPass(GameRenderer *renderer)
    : RenderPass(renderer),
      m_background(new GameBackground(*renderer->parentGameScene(), this)),
      m_atmosphereRenderer(new AtmosphereRenderer(renderer->parentGameScene()->getCameraman().camera(), this)),
      m_program(),
      m_framebuffer(new Framebuffer(this))
{
    auto gameContent = renderer->parentGameScene()->parentGameContent();
    gameContent->addOnSizeChangedListener([this](int width, int height) {
        m_framebuffer->resizeAttachments(width, height);
    });

    auto sceneTexture = new Texture2D(m_framebuffer);
    sceneTexture->setFormat(Texture::RGBA16F); // TODO: rgb?
    sceneTexture->bind();
    sceneTexture->setParams(Texture2D::defaultParams());
    sceneTexture->unbind();

    auto rbo = new Renderbuffer(m_framebuffer);
    rbo->bind();
    rbo->setFormat(Texture::DepthComponent24);
    rbo->unbind();

    m_framebuffer->bind();
    m_framebuffer->getActiveOutputList().addColor(0);
    m_framebuffer->update();
    m_framebuffer->attachRenderbuffer(rbo, Framebuffer::DepthAttachment);
    m_framebuffer->attachTexture(sceneTexture, Framebuffer::ColorAttachmentZero);
}

void GameRenderPass::render() {
    m_framebuffer->bind();
    m_framebuffer->clear(Framebuffer::ColorBuffer | Framebuffer::DepthBuffer);

    // TODO
//    Framebuffer::getDefault()->bind();
//    Framebuffer::getDefault()->clear(Framebuffer::ColorBuffer | Framebuffer::DepthBuffer);

    m_program->bind();

    renderPlanets();
    renderParticles();
    renderSpacecraft();
    renderBackground();
    renderAtmosphere();
}

Texture2D* GameRenderPass::getOutput() {
    return m_framebuffer->getAttachedTexture2D(Framebuffer::ColorAttachmentZero);
}

LoaderConfig GameRenderPass::resourceLoaderConfig() {
    return LoaderConfig::create<GameRenderPass>({
        .loader = [this] { loadResources(); },
        .children = {m_background, m_atmosphereRenderer},
        .useContext = true
    });
}

void GameRenderPass::loadResources() {
    // create shaders
    ShaderBuilder vertex(Shader::Type::Vertex);
    vertex.setPath("shaders/vertexShader.glsl");

    ShaderBuilder fragment(Shader::Type::Fragment);
    fragment.setPath("shaders/fragmentShader.glsl");

    ShaderProgramBuilder programCreator;
    programCreator.setParent(this);
    programCreator.addBuilder(vertex);
    programCreator.addBuilder(fragment);

    m_program = programCreator.get();
    m_program->bind();
    m_program->loadActiveLocations();
}

Cameraman& GameRenderPass::cameraman() const {
    return parentGameRenderer()->parentGameScene()->getCameraman();
}

void GameRenderPass::updateMatrices(const glm::mat4 &transformation) {
    auto &camera = cameraman().camera();
    m_program->setMat4("MVPMatrix", camera.getProjectionMatrix() * camera.getViewMatrix() * transformation);
    m_program->setMat4("modelView", camera.getViewMatrix() * transformation);
}

void GameRenderPass::applyMaterial(const Material &material) {
    m_program->setFloat("shininess", material.getFloat(Material::Shininess, 1.3f));
    m_program->setFloat("specularStrength", material.getFloat(Material::SpecularStrength, 0.08f));
    m_program->setFloat("diffuseStrength", material.getFloat(Material::DiffuseStrength, 0.13f));
    m_program->setFloat("ambientStrength", material.getFloat(Material::AmbientStrength, 0.4f));
}

bool GameRenderPass::isPlanetVisible(Planet *planet) {
    constexpr float drawPlanets = 0.3;
    return planet->getIndex() == 0 || cameraman().getAnimationPos() >= drawPlanets;
}

void GameRenderPass::renderPlanets() {
    auto &planets = parentGameRenderer()->parentGameScene()->getPlanets();

    planets.forEach([this](Planet *planet) {
        if (!isPlanetVisible(planet))
            return;

        planet->update();
        planet->transform();

        updateMatrices(planet->transformation());

        planet->getTexture()->use(0);

        auto shape = planet->getShape();
        shape->getInputLayout(0)->bind();

        for (auto &mesh : shape->getMeshes()) {
            applyMaterial(mesh.material);
            Engine::drawArrays(mesh.start, mesh.count);
        }

        renderOrbit(planet->findChild<PlanetOrbit*>(FindOption::Direct));
    });
}

void GameRenderPass::renderOrbit(PlanetOrbit *orbit) {
    if (orbit == nullptr)
        return;

    orbit->translate(); // TODO: do we really need all these calls?
    orbit->rotate();
    orbit->transform();

    auto platformChunkShape = orbit->getShape();
    platformChunkShape->getInputLayout(0)->bind();

    for (auto &iChunk : orbit->chunks()) {
        updateMatrices(iChunk.getTransform());

        for (auto &mesh : platformChunkShape->getMeshes()) {
            applyMaterial(mesh.material);
            Engine::drawElements(mesh.start, mesh.count);
        }
    }
}

void GameRenderPass::renderParticles() {
    auto &crystalParticles = parentGameRenderer()->parentGameScene()->getCrystalParticles();
    crystalParticles.update();

    for (auto &particle : crystalParticles) {
        updateMatrices(crystalParticles.getTransform(particle));

        auto planet = CrystalParticleSystem::getParticleParent(particle);
        planet->getTexture()->use(0);

        auto shape = crystalParticles.getShape(particle.shapeId);
        shape->getInputLayout(0)->bind();

        for (auto &mesh : shape->getMeshes()) {
            applyMaterial(mesh.material);
            Engine::drawArrays(mesh.start, mesh.count);
        }
    }
}

void GameRenderPass::renderSpacecraft() {
    auto spacecraft = parentGameRenderer()->parentGameScene()->getSpacecraft();

    // TODO: remove runtime scaling
    spacecraft->setScale(glm::vec3(0.025));
    spacecraft->scale();
    spacecraft->transform();

    updateMatrices(spacecraft->transformation());

    auto spacecraftShape = spacecraft->getShape();
    spacecraftShape->getInputLayout(0)->bind();

    for (auto &mesh : spacecraftShape->getMeshes()) {
        applyMaterial(mesh.material);
        Engine::drawElements(mesh.start, mesh.count);
    }
}

void GameRenderPass::renderBackground() {
    m_background->animate();
    m_background->draw(m_program);
}

void GameRenderPass::renderAtmosphere() {
    auto &planets = parentGameRenderer()->parentGameScene()->getPlanets();

    glEnable(GL_BLEND); // TODO: move it to Engine
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_atmosphereRenderer->begin();

    planets.forEach([this](Planet *planet) {
        if (!isPlanetVisible(planet) || !planet->hasAtmosphere())
            return;

        m_atmosphereRenderer->render(planet);
    });

    glDisable(GL_BLEND);
}
