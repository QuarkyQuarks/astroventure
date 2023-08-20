#ifndef ASTROVENTURE_ROTATINGORBITSWIDGET_H
#define ASTROVENTURE_ROTATINGORBITSWIDGET_H

#include <algine/core/widgets/Widget.h>
#include <algine/core/painter/Painter.h>
#include <algine/core/texture/Texture2D.h>

class RotatingOrbitsWidget : public algine::Widget {
    STATIC_INITIALIZER_DECL

public:
    RotatingOrbitsWidget();

private:
    void onDraw(algine::Painter &painter) override;

private:
    struct OrbitParameters {
        algine::Texture2D *texture;
        float angularVelocity;
    };

    OrbitParameters m_orbit1;
    OrbitParameters m_orbit2;
    OrbitParameters m_orbit3;

    algine::Texture2D *m_planetTex {};
};

#endif //ASTROVENTURE_ROTATINGORBITSWIDGET_H
