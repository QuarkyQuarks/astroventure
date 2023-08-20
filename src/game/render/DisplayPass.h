#ifndef ASTROVENTURE_DISPLAYPASS_H
#define ASTROVENTURE_DISPLAYPASS_H

#include <algine/core/renderer/RenderPass.h>
#include <algine/core/texture/Texture2D.h>
#include <algine/core/shader/ShaderProgram.h>

#include "GameRenderer.h"

using namespace algine;

class DisplayPass: public GameRenderer::RenderPass, public Loadable {
public:
    explicit DisplayPass(GameRenderer *parent);

    void setTexture(Texture2D *texture);

    void render() override;

    LoaderConfig resourceLoaderConfig() override;

private:
    void loadResources();

private:
    Texture2D *m_texture;
    ShaderProgram *m_program;
};


#endif //ASTROVENTURE_DISPLAYPASS_H
