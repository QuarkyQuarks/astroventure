#ifndef ASTROVENTURE_BLOOMPASS_H
#define ASTROVENTURE_BLOOMPASS_H

#include <algine/core/Framebuffer.h>
#include <algine/core/shader/ShaderProgram.h>
#include <algine/std/Blur.h>

#include "GameRenderer.h"

class BloomPass: public GameRenderer::RenderPass, public Loadable {
public:
    explicit BloomPass(GameRenderer *renderer);

    void setInput(Texture2D *texture);

    void render() override;

    Texture2D* getOutput();

    LoaderConfig resourceLoaderConfig() override;

private:
    void loadResources();

private:
    float m_multiplier;
    ShaderProgram *m_searchProgram;
    Framebuffer *m_searchFb;
    Blur *m_blur;
    Texture2D *m_texture;

private:
    glm::ivec2 m_dimensions;
    glm::ivec2 m_blurDimensions;
};

#endif //ASTROVENTURE_BLOOMPASS_H
