#include "MenuLayer.h"
#include "game/ui/AnimTools.h"
#include "game/ui/GameUIScene.h"
#include "game/render/GameRenderer.h"
#include "game/GameScene.h"
#include "game/GameContent.h"

#include <algine/core/widgets/Container.h>
#include <algine/core/widgets/animation/OpacityAnimation.h>
#include <algine/core/painter/Painter.h>

namespace UI {
MenuLayer::MenuLayer(GameUIScene *scene, std::string_view file)
    : Layer(scene),
      m_background()
{
    cfgBackground();

    auto container = Widget::constructFromXMLFile<Container*>(file.data(), this);
    setContainer(container);

    container->setOpacity(0.0f);
    container->setBackground(Paint(m_background->get()));
}

class MenuLayer::MenuAnimation: public Widgets::OpacityAnimation {
public:
    enum class Type {
        Open, Close
    };

public:
    explicit MenuAnimation(MenuLayer *layer): Widgets::OpacityAnimation() {
        m_layer = layer;
        m_type = layer->isFlagEnabled(MenuLayer::Flag::Open) ? Type::Open : Type::Close;
        setInterpolator(AnimTools::fadeInInterp());
        setDuration(200);
    }

    MenuAnimation(MenuLayer *layer, float from, float to): MenuAnimation(layer) {
        setFrom(from);
        setTo(to);
    }

    Type getType() const {
        return m_type;
    }

protected:
    void onFinish() override {
        if (m_layer->isFlagEnabled(MenuLayer::Flag::Close)) {
            m_layer->setFlag(MenuLayer::Flag::Close, false);
            m_layer->hide();
        }
    }

private:
    MenuLayer *m_layer;
    Type m_type;
};

void MenuLayer::onShow() {
    Layer::onShow();
    setFlag(Flag::Open);
    requestBackgroundUpdate();
    unblockClose();
    getContainer()->addAnimation(std::make_unique<MenuAnimation>(this, getOpacity(), 1.0f));
}

void MenuLayer::setFlag(Flag flag, bool enabled) {
    m_flags.set(static_cast<size_t>(flag), enabled);
}

bool MenuLayer::isFlagEnabled(Flag flag) const {
    return m_flags.test(static_cast<size_t>(flag));
}

void MenuLayer::draw(const WidgetDisplayOptions &options) {
    if (shouldUpdateBackground()) {
        cancelBackgroundUpdate();
        updateBackground(options);
    }

    Layer::draw(options);
}

void MenuLayer::setCloseWidget(Widget *widget, const CloseAction &closeAction) {
    widget->setEventListener(Event::Click, [this, closeAction](Widget*, const Event&) {
        if (!close())
            return;

        if (closeAction) {
            closeAction();
        }
    });
}

bool MenuLayer::close() {
    if (!canBeClosed())
        return false;

    auto &animations = getContainer()->getAnimations();
    auto anim = animations.empty() ? nullptr : dynamic_cast<MenuAnimation*>(animations.front().get());

    if (anim && anim->getType() == MenuAnimation::Type::Close)
        return false;

    setFlag(Flag::Open, false);
    setFlag(Flag::Close);

    auto closeAnim = std::make_unique<MenuAnimation>(this, getOpacity(), 0.0f);

    if (anim) {
        closeAnim->start(1.0f - anim->getPos());
        anim->stop();
    } else {
        closeAnim->start();
    }

    getContainer()->addAnimation(std::move(closeAnim));

    return true;
}

void MenuLayer::blockClose() {
    setFlag(Flag::BlockClose);
}

void MenuLayer::unblockClose() {
    setFlag(Flag::BlockClose, false);
}

bool MenuLayer::isCloseBlocked() const {
    return isFlagEnabled(Flag::BlockClose);
}

bool MenuLayer::canBeClosed() const {
    return !isCloseBlocked() && !isFlagEnabled(Flag::Close);
}

void MenuLayer::cfgBackground() {
    TextureCreateInfo createInfo;
    createInfo.format = Texture::RGB16F; // TODO: maybe just RGB8?
    createInfo.params = Texture2D::defaultParams();

    m_background = new Blur({
        .textureCreateInfo = std::move(createInfo),
        .kernelRadius = 18,
        .kernelSigma = 16,
        .blurComponents = "rgb"
    }, this);

    auto gameUIScene = parentGameUIScene();
    auto quadRenderer = gameUIScene->parentGameRenderer()->getQuadRenderer();

    m_background->setQuadRenderer(quadRenderer);

    gameUIScene->addOnSizeChangedListener([this](int width, int height) {
        constexpr auto s_base = 500.0f * 989.0f;
        auto s_current = static_cast<float>(width * height);
        auto m = sqrtf(s_base / s_current);
        auto multiplier = 0.08f * m;

        m_background->resizeOutput(
            static_cast<int>((float) width * multiplier),
            static_cast<int>((float) height * multiplier));

        requestBackgroundUpdate();
    });
}

void MenuLayer::updateBackground(const WidgetDisplayOptions &options) {
    auto painter = options.painter;
    auto wasPainterActive = painter->isActive();

    if (wasPainterActive)
        painter->end();

    Engine::setViewport(m_background->getViewport());

    auto tex = options.framebuffer->getAttachedTexture2D(Framebuffer::ColorAttachmentZero);

    m_background->getQuadRenderer()->getInputLayout()->bind();
    m_background->makeBlur(tex);

    getContainer()->invalidate();

    if (wasPainterActive) {
        painter->begin();
    }
}

void MenuLayer::requestBackgroundUpdate() {
    setFlag(Flag::BackgroundUpdateRequired);
}

void MenuLayer::cancelBackgroundUpdate() {
    setFlag(Flag::BackgroundUpdateRequired, false);
}

bool MenuLayer::shouldUpdateBackground() const {
    return isFlagEnabled(Flag::BackgroundUpdateRequired);
}
} // UI
