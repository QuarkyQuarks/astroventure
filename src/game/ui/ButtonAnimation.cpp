#include "ButtonAnimation.h"
#include "AnimTools.h"
#include "game/scheme/ColorSchemeManager.h"
#include "game/GameScene.h"

#include <algine/core/widgets/Widget.h>

namespace UI {
ButtonAnimation::ButtonAnimation(Type type)
    : m_type(type)
{
    switch (type) {
        case Type::Press:
        case Type::Release:
            setDuration(200);
            setInterpolator(AnimTools::fadeInInterp());
            break;
        case Type::Click:
            setDuration(300);
            setInterpolator(AnimTools::fadeInOutInterp());
            break;
    }
}

// TODO: color theme

void ButtonAnimation::onStart() {
    auto widget = getWidget();

    // try to find a parent game scene & get a color scheme
    auto scene = widget->findParent<GameScene*>();
    assert(scene != nullptr);

    auto &colorScheme = scene->getColorSchemeManager().getColorScheme();

    const Color &c1 = colorScheme.btnBackground;
    const Color &c2 = colorScheme.btnPressedBackground;

    if (m_type == Type::Click) {
        if (widget->hasAnimation(Event::Release)) {
            auto &anim = widget->getAnimation(Event::Release);

            if (anim->isActive()) {
                setPos((1.0f - anim->getPos()) / 2.0f);
                anim->stop();
            }
        }

        if (widget->hasAnimation(Event::Press)) {
            auto &anim = widget->getAnimation(Event::Press);

            if (anim->isActive()) {
                setPos(anim->getPos() / 2.0f);
                anim->stop();
            }
        }

        setFrom(c1);
        setTo(c2);
    } else {
        Event::Id inv = m_type == Type::Press ? Event::Release : Event::Press;

        if (widget->hasAnimation(inv)) {
            auto &anim = widget->getAnimation(inv);

            if (anim->isActive()) {
                setPos(1.0f - anim->getPos());
                anim->stop();
            }
        }

        if (m_type == Type::Press) {
            setFrom(c1);
            setTo(c2);
        } else {
            setFrom(c2);
            setTo(c1);
        }
    }
}
} // UI
