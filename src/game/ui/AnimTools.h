#ifndef ASTROVENTURE_ANIMTOOLS_H
#define ASTROVENTURE_ANIMTOOLS_H

#include <algine/core/math/CubicSpline.h>
#include <algine/core/widgets/Widget.h>

using namespace algine;

namespace UI {
class AnimTools {
public:
    static void setButtonAnimation(Widget *widget);

    static const Widgets::InterpolatorPtr& fadeInOutInterp();
    static const Widgets::InterpolatorPtr& fadeInInterp();

    static CubicSpline fadeInOut();
    static CubicSpline fadeIn();
    static CubicSpline fadeOut();

private:
    static Widgets::InterpolatorPtr m_fadeInOutInterp;
    static Widgets::InterpolatorPtr m_fadeInInterp;
};
}

#endif //ASTROVENTURE_ANIMTOOLS_H
