#ifndef ASTROVENTURE_GAMERENDERPASS_H
#define ASTROVENTURE_GAMERENDERPASS_H

#include <algine/core/Framebuffer.h>
#include <algine/std/Material.h>

#include "GameRenderer.h"
#include "game/Cameraman.h"
#include "game/GameBackground.h"
#include "game/planet/PlanetOrbit.h"
#include "game/planet/AtmosphereRenderer.h"

class GameRenderPass: public GameRenderer::RenderPass, public Loadable {
public:
    explicit GameRenderPass(GameRenderer *renderer);

    void render() override;

    Texture2D* getOutput();

    LoaderConfig resourceLoaderConfig() override;

private:
    Cameraman& cameraman() const;

private:
    void loadResources();

    void updateMatrices(const glm::mat4 &transformation);
    void applyMaterial(const Material &material);

    bool isPlanetVisible(Planet *planet);

    void renderPlanets();
    void renderOrbit(PlanetOrbit *orbit);
    void renderParticles();
    void renderSpacecraft();

    /**
     * It must be drawn after all the solid geometry,
     * since it will be drawn with the depth mask disabled
     */
    void renderBackground();

    /**
     * Since atmosphere has an alpha channel, it
     * must be drawn after the all solid geometry
     */
    void renderAtmosphere();

private:
    GameBackground *m_background;
    AtmosphereRenderer *m_atmosphereRenderer;

private:
    ShaderProgram *m_program;
    Framebuffer *m_framebuffer;
};


#endif //ASTROVENTURE_GAMERENDERPASS_H
