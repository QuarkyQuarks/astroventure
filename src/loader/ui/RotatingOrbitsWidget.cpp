#include "RotatingOrbitsWidget.h"

#include <algine/core/painter/Paint.h>
#include <algine/core/Engine.h>
#include <algine/core/TypeRegistry.h>
#include <algine/core/widgets/Dimen.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

using namespace algine;

STATIC_INITIALIZER_IMPL(RotatingOrbitsWidget) {
    alRegisterType(RotatingOrbitsWidget);
}

RotatingOrbitsWidget::RotatingOrbitsWidget()
    : m_orbit1({.angularVelocity = 1.8f}),
      m_orbit2({.angularVelocity = -2.3f}),
      m_orbit3({.angularVelocity = 1.5f})
{
    auto loadTexture = [this](Texture2D *&texture, const std::string &path) {
        texture = new Texture2D(this);
        texture->bind();
        texture->fromFile(path);
    };

    loadTexture(m_planetTex, "textures/planet.png");
    loadTexture(m_orbit1.texture, "textures/orbit1.png");
    loadTexture(m_orbit2.texture, "textures/orbit2.png");
    loadTexture(m_orbit3.texture, "textures/orbit3.png");
}

void RotatingOrbitsWidget::onDraw(Painter &painter) {
    // TODO: precalculate this value
    const auto widgetSize = Dimen(128, Unit::dp).pixels(getParentWindow()); // width & height

    auto drawOrbit = [widgetSize, &painter](OrbitParameters &orbit) {
        float angle = fmodf((float) Engine::timeFromStart() / 1000.0f * orbit.angularVelocity, 2 * glm::pi<float>());

        glm::mat4 translation = glm::translate(glm::mat4(1.0f), {widgetSize / 2, widgetSize / 2, 0.0f});
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, {0.0f, 0.0f, 1.0f});

        painter.setTransform(translation * rotation);

        painter.drawTexture(orbit.texture, {
            {-widgetSize / 2, -widgetSize / 2},
            { widgetSize / 2,  widgetSize / 2}
        });
    };

    painter.setTransform(glm::mat4(1.0f));
    painter.drawTexture(m_planetTex, {{0, 0}, {widgetSize, widgetSize}});

    drawOrbit(m_orbit1);
    drawOrbit(m_orbit2);
    drawOrbit(m_orbit3);
}