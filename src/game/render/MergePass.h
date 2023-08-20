#ifndef ASTROVENTURE_MERGEPASS_H
#define ASTROVENTURE_MERGEPASS_H

#include <algine/core/texture/Texture2D.h>
#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/Framebuffer.h>

#include "GameRenderer.h"

class MergePass: public GameRenderer::RenderPass, public Loadable {
public:
    explicit MergePass(GameRenderer *renderer);

    void setInputs(Texture2D *base, Texture2D *bloom);

    Texture2D* getOutput();
    Framebuffer* getOutputFramebuffer();

    void render() override;

    LoaderConfig resourceLoaderConfig() override;

private:
    void loadResources();

private:
    Texture2D *m_base;
    Texture2D *m_bloom;
    Framebuffer *m_framebuffer;
    ShaderProgram *m_program;
};

#endif //ASTROVENTURE_MERGEPASS_H
