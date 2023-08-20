#ifndef ASTROVENTURE_BUTTONANIMATION_H
#define ASTROVENTURE_BUTTONANIMATION_H

#include <algine/core/widgets/animation/ColorAnimation.h>

using namespace algine;

namespace UI {
class ButtonAnimation: public Widgets::ColorAnimation {
public:
    enum class Type {
        Press, Release, Click
    };

public:
    explicit ButtonAnimation(Type type);

protected:
    void onStart() override;

private:
    Type m_type;
};
} // UI

#endif //ASTROVENTURE_BUTTONANIMATION_H
