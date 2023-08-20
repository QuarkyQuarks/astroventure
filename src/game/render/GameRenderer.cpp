#include "GameRenderer.h"
#include "GameRenderPass.h"
#include "DisplayPass.h"
#include "BloomPass.h"
#include "MergePass.h"
#include "UIPass.h"
#include "game/GameScene.h"

#include <algine/core/assert_cast.h>

GameRenderer::GameRenderer(GameScene *scene)
    : Renderer(scene),
      m_quadRenderer(new QuadRenderer(this))
{
    addPass(new GameRenderPass(this));
    addPass(new BloomPass(this));
    addPass(new MergePass(this));
    addPass(new UIPass(this));
    addPass(new DisplayPass(this));
}

GameScene* GameRenderer::parentGameScene() const {
    return assert_cast<GameScene*>(getParent());
}

QuadRenderer* GameRenderer::getQuadRenderer() const {
    return m_quadRenderer;
}

LoaderConfig GameRenderer::resourceLoaderConfig() {
    auto children = findChildren<Loadable*>(FindOption::Direct);
    return LoaderConfig::create<GameRenderer>({
        .loader = [this] {
            auto gameRenderPass = findChild<GameRenderPass*>();
            auto bloomPass = findChild<BloomPass*>();
            auto mergePass = findChild<MergePass*>();
            auto uiPass = findChild<UIPass*>();
            auto displayPass = findChild<DisplayPass*>();

            bloomPass->setInput(gameRenderPass->getOutput());
            mergePass->setInputs(gameRenderPass->getOutput(), bloomPass->getOutput());
            uiPass->setInput(mergePass->getOutputFramebuffer());
            displayPass->setTexture(uiPass->getOutput());
        },
        .children = {children.begin(), children.end()},
        .dependsOn = {children.begin(), children.end()}
    });
}

GameRenderer::RenderPass::RenderPass(Renderer *parent)
    : algine::RenderPass(parent) {}

GameRenderer* GameRenderer::RenderPass::parentGameRenderer() const {
    return assert_cast<GameRenderer*>(getParent());
}
