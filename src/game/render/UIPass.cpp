#include "UIPass.h"

UIPass::UIPass(GameRenderer *renderer)
    : GameRenderer::RenderPass(renderer),
      m_uiScene(new UI::GameUIScene(this)) {}

void UIPass::setInput(Framebuffer *base) {
    m_uiScene->setFramebuffer(base);
}

Texture2D* UIPass::getOutput() {
    return m_uiScene->getFramebuffer()->getAttachedTexture2D(Framebuffer::ColorAttachmentZero);
}

void UIPass::render() {
    m_uiScene->draw();
}
