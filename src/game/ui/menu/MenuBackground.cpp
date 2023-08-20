#include "MenuBackground.h"

#include <algine/core/PtrMaker.h>

namespace UI {
Texture2D* MenuBackground::m_base;
Blur* MenuBackground::m_blur;
float MenuBackground::m_multiplier;

void MenuBackground::init(Texture2D *base, QuadRenderer *quadRenderer, int width, int height) {
    calcMultiplier(width, height);

    m_base = base;

    TextureCreateInfo createInfo;
    createInfo.format = Texture::RGB16F;
    createInfo.width = MenuBackground::width(width);
    createInfo.height = MenuBackground::height(height);
    createInfo.params = Texture2D::defaultParams();

//    m_blur = new Blur(createInfo); // TODO: parent
//    m_blur->configureShaders(18, "rgb");
//    m_blur->setQuadRenderer(quadRenderer);
//    m_blur->configureKernel(18, 16);
//    m_blur->setAmount(2);
}

void MenuBackground::update() {
    m_blur->makeBlur(m_base);
}

Texture2D* MenuBackground::get() {
    return m_blur->get();
}

void MenuBackground::onSizeChanged(int width, int height) {
    calcMultiplier(width, height);
    //m_blur->resizeOutput(MenuBackground::width(width), MenuBackground::height(height));
}

int MenuBackground::width(int width) {
    return static_cast<int>((float) width * m_multiplier);
}

int MenuBackground::height(int height) {
    return static_cast<int>((float) height * m_multiplier);
}

void MenuBackground::calcMultiplier(int width, int height) {
    auto s_base = 500.0f * 989.0f;
    auto s_current = static_cast<float>(width * height);
    auto m = sqrtf(s_base / s_current);
    m_multiplier = 0.08f * m;
}
} // UI
