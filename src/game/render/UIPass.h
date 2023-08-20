#ifndef ASTROVENTURE_UIPASS_H
#define ASTROVENTURE_UIPASS_H

#include <algine/core/Framebuffer.h>

#include "GameRenderer.h"
#include "game/ui/GameUIScene.h"

class UIPass: public GameRenderer::RenderPass {
public:
    explicit UIPass(GameRenderer *renderer);

    void setInput(Framebuffer *base);

    Texture2D* getOutput();

    void render() override;

private:
    UI::GameUIScene *m_uiScene;
};

#endif //ASTROVENTURE_UIPASS_H
