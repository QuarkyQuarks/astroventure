#ifndef ASTROVENTURE_CAMERAMAN_H
#define ASTROVENTURE_CAMERAMAN_H

#include <algine/std/Camera.h>
#include <algine/core/math/BezierCurve.h>

#include <glm/trigonometric.hpp>

#include "loader/Loadable.h"

using namespace algine;

class GameScene;

class Cameraman: public Loadable {
public:
    enum class Animation {
        Home,
        Intro,
        Game
    };

    using Trajectory = algine::BezierCurve<glm::vec3>;

public:
    constexpr static glm::vec3 HomeScreenPos {1.0f, -1.23f, -3.9f};
    constexpr static glm::vec3 GamePos {0.0f, 0.0f, 7.5f};
    constexpr static float FieldOfView = glm::radians(10.0f);
    constexpr static float Near = 1.0f;
    constexpr static float Far = 64.0f;

public:
    explicit Cameraman(GameScene &scene);

    const Camera& getCamera() const;
    Camera& camera();
    float getAnimationPos() const;

    void animate();

    void startAnimation(Animation animation);
    Animation currentAnimation() const;

    LoaderConfig resourceLoaderConfig() override;

private:
    void (Cameraman::*m_step_ptr)();

    void intro();
    void home();
    void game();

    void move();

private:
    GameScene &m_scene;

private:
    Camera m_camera;

    Trajectory m_camTrajectory;
    Trajectory m_camTargetTrajectory;

    Animation m_animation;

    glm::vec3 m_cameraTargetPos;

private:
    float m_animationStartTime {0.0f};
    float m_animationPos {0.0f};
};

#endif
