#include "MenuLayer.h"
#include "game/ui/AnimTools.h"
#include "MenuBackground.h"

#include <algine/core/widgets/Container.h>
#include <algine/core/widgets/animation/OpacityAnimation.h>

namespace UI {
MenuLayer::MenuLayer(GameUIScene *scene, std::string_view file)
    : Layer(scene)
{
    auto container = Widget::constructFromXMLFile<Container*>(file.data(), this);
    setContainer(container);

    container->setOpacity(0.0f);
    // container->setBackground(Paint(MenuBackground::get())); TODO
}

class MenuAnimation: public Widgets::OpacityAnimation {
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
    getContainer()->addAnimation(std::make_unique<MenuAnimation>(this, getOpacity(), 1.0f));
}

bool MenuLayer::isAnimationActive() const {
    return !getContainer()->getAnimations().empty();
}

void MenuLayer::setFlag(Flag flag, bool enabled) {
    m_flags.set(static_cast<size_t>(flag), enabled);
}

bool MenuLayer::isFlagEnabled(Flag flag) const {
    return m_flags.test(static_cast<size_t>(flag));
}

void MenuLayer::setCloseWidget(Widget *widget, const CloseAction &closeAction) {
    widget->setEventListener(Event::Click, [this, closeAction](Widget*, const Event&) {
        auto &animations = getContainer()->getAnimations();
        auto anim = animations.empty() ? nullptr : dynamic_cast<MenuAnimation*>(animations.front().get());

        if (anim && anim->getType() == MenuAnimation::Type::Close)
            return;

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

        if (closeAction) {
            closeAction();
        }
    });
}
} // UI
