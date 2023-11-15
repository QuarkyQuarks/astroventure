#include "Cameraman.h"
#include "GameScene.h"
#include "GameContent.h"
#include "math/VecTools.h"

#include <algine/core/Engine.h>

using namespace algine;

Cameraman::Cameraman(GameScene &scene)
    : m_scene(scene),
      m_step_ptr(nullptr),
      m_cameraTargetPos(),
      m_animation(Animation::Home)
{
    m_camera.setNear(Near);
    m_camera.setFar(Far);
    m_camera.setFieldOfView(FieldOfView);

    scene.parentGameContent()->addOnSizeChangedListener([this](int width, int height) {
        m_camera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
        m_camera.perspective();
    });
}

const Camera& Cameraman::getCamera() const {
    return m_camera;
}

algine::Camera& Cameraman::camera() {
    return m_camera;
}

float Cameraman::getAnimationPos() const {
    return m_animationPos;
}

void Cameraman::home() {
    glm::vec2 orientation = vectorOrientation(m_cameraTargetPos - HomeScreenPos);
    m_camera.setPos(HomeScreenPos);
    m_camera.setPitch(orientation.x);
    m_camera.setYaw(orientation.y);
    m_camera.setRoll(0);
    move();
}

void Cameraman::intro() {
    if (m_animationStartTime == 0.0f) {
        m_animationStartTime = m_scene.getGameTime();
        return;
    }

    auto currentFrame = m_scene.getGameTime();
    m_animationPos = glm::min((currentFrame - m_animationStartTime) / 1700.0f, 1.0f);

    m_camera.setPos(m_camTrajectory.getPoint(m_animationPos));
    m_cameraTargetPos = m_camTargetTrajectory.getPoint(m_animationPos);

    glm::vec2 orientation = vectorOrientation(m_cameraTargetPos - m_camera.getPos());
    m_camera.setPitch(orientation.x);
    m_camera.setYaw(orientation.y);
    m_camera.setRoll(0);

    if (m_animationPos == 1.0f) {
        startAnimation(Animation::Game);
    } else {
        move();
    }
}

void Cameraman::game() {
    m_camera.setPos(GamePos);
    m_camera.setRotate(0.0f, 0.0f, 0.0f);
    move();
}

void Cameraman::move() {
    m_camera.translate();
    m_camera.rotate();
    m_camera.updateMatrix();
}

void Cameraman::animate() {
    if (m_step_ptr != nullptr) {
        (this->*m_step_ptr)();
    }
}

void Cameraman::startAnimation(Animation animation) {
    m_animation = animation;

    switch (m_animation) {
        case Animation::Home: {
            constexpr glm::vec3 regulationPoint1(2.7f, -1.23f, 1.5f);
            constexpr glm::vec3 regulationPoint2(0.8f, -1.23f, 2.5f);
            m_camTrajectory.setPoints({HomeScreenPos, regulationPoint1, regulationPoint2, GamePos});

            auto &planet0Pos = m_scene.getPlanets()[0]->getPos();

            constexpr glm::vec3 deltaTargetPos(0.0f, 0.2f, 0.0f);
            m_cameraTargetPos = planet0Pos + deltaTargetPos;

            constexpr glm::vec3 finalTargetPos(0.0f, 0.0f, 0.0f);
            glm::vec3 regulationPoint3 = planet0Pos + glm::vec3(0.0f, -0.4f, 0.0f);
            m_camTargetTrajectory.setPoints({m_cameraTargetPos, regulationPoint3, finalTargetPos});

            home();

            m_step_ptr = nullptr;
            m_animationPos = 0.0f;

            break;
        }

        case Animation::Intro:
            m_animationStartTime = 0.0f;
            m_step_ptr = &Cameraman::intro;
            break;

        case Animation::Game:
            game();
            m_step_ptr = nullptr;
            break;
    }
}

Cameraman::Animation Cameraman::currentAnimation() const {
    return m_animation;
}

LoaderConfig Cameraman::resourceLoaderConfig() {
    return LoaderConfig::create<Cameraman>({
        .loader = [this] {
            startAnimation(Animation::Home);
        },
        .dependsOn = {&m_scene}
    });
}
