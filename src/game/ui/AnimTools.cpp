#include "AnimTools.h"
#include "ButtonAnimation.h"
#include <algine/core/widgets/animation/CubicSplineInterpolator.h>

using namespace Widgets;

namespace UI {
Widgets::InterpolatorPtr AnimTools::m_fadeInOutInterp {nullptr};
Widgets::InterpolatorPtr AnimTools::m_fadeInInterp {nullptr};

void AnimTools::setButtonAnimation(Widget *widget) {
    widget->setEventAnimation(Event::Press, Animation::create<ButtonAnimation>(ButtonAnimation::Type::Press));
    widget->setEventAnimation(Event::Release, Animation::create<ButtonAnimation>(ButtonAnimation::Type::Release));
    widget->setEventAnimation(Event::Click, Animation::create<ButtonAnimation>(ButtonAnimation::Type::Click));
}

const Widgets::InterpolatorPtr& AnimTools::fadeInOutInterp() {
    if (!m_fadeInOutInterp)
        m_fadeInOutInterp = Interpolator::create<CubicSplineInterpolator>(AnimTools::fadeInOut());
    return m_fadeInOutInterp;
}

const Widgets::InterpolatorPtr& AnimTools::fadeInInterp() {
    if (!m_fadeInInterp)
        m_fadeInInterp = Interpolator::create<CubicSplineInterpolator>(AnimTools::fadeIn());
    return m_fadeInInterp;
}

CubicSpline AnimTools::fadeInOut() {
    return CubicSpline({
        {0.0f, 0.0f},
        {0.25f, 0.25f},
        {0.5f, 1.0f},
        {0.75f, 0.25f},
        {1.0f, 0.0f}
    });
}

CubicSpline AnimTools::fadeIn() {
    return CubicSpline({
        {0.0f, 0.0f},
        {0.5f, 0.25f},
        {1.0f, 1.0f},
        {1.5f, 0.25f},
        {2.0f, 0.0f}
    });
}

CubicSpline AnimTools::fadeOut() {
    return CubicSpline({
       {-1.0f, 0.0f},
       {-0.5f, 0.25f},
       { 0.0f, 1.0f},
       { 0.5f, 0.25f},
       { 1.0f, 0.0f}
   });
}
}
