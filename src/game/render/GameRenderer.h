#ifndef ASTROVENTURE_GAMERENDERER_H
#define ASTROVENTURE_GAMERENDERER_H

#include <algine/core/renderer/Renderer.h>
#include <algine/core/renderer/RenderPass.h>
#include <algine/std/QuadRenderer.h>

#include "loader/Loadable.h"

using namespace algine;

class GameScene;

class GameRenderer: public Renderer, public Loadable {
public:
    class RenderPass;

public:
    explicit GameRenderer(GameScene *scene);

    GameScene* parentGameScene() const;
    QuadRenderer* getQuadRenderer() const;

    LoaderConfig resourceLoaderConfig() override;

private:
    QuadRenderer *m_quadRenderer;
};

class GameRenderer::RenderPass: public algine::RenderPass {
public:
    explicit RenderPass(Renderer *parent);

    GameRenderer* parentGameRenderer() const;
};

#endif //ASTROVENTURE_GAMERENDERER_H
