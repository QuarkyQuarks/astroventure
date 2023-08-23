#ifndef ASTROVENTURE_VECTOOLS_H
#define ASTROVENTURE_VECTOOLS_H

#include "MathConstants.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>

#include <cmath>

// TODO: split declarations & definitions

/// returns angle([-Pi, Pi]) between vectors considering appropriate orientation
template<typename T>
T angleBetweenVectors(glm::vec<2, T> vector1, glm::vec<2, T> vector2) {
    T dot = vector1.x * vector2.x + vector1.y * vector2.y;
    T det = vector1.x * vector2.y - vector2.x  * vector1.y;
    T angle = std::atan2(det, dot);
    return angle;
}

/// determines yaw and pitch of the vector
inline glm::vec2 vectorOrientation(const glm::vec3& vector) {
    glm::vec3 normal = normalize(vector);
    return {asin(-normal.y), -(atan2(normal.x, normal.z) + PI)};
}

/// returns equivalent to the provided angle within the bounds [0, 2Pi]
template<typename T>
T adjustedAngle(T angle0) {
    T angle = std::fmod(angle0, 2 * glm::pi<T>());
    return angle < 0 ? angle + 2 * glm::pi<T>() : angle;
}

/// cubic interpolation between x [0, 1] (0 -> y1, 1 -> y2)
template<typename T, typename L>
T mixCubic(const T &p0, const T &p1, const T &p2, const T &p3, L x) {
    return p1 + 0.5 * x * (p2 - p0 + x * (2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3 + x * (3.0 * (p1 - p2) + p3 - p0)));
}

/**
 * Calculate the normal for given vertices <b>lying on a sphere</b>
 * @param p1 vertex 1
 * @param p2 vertex 2
 * @param p3 vertex 3
 * @return pair\<normal, orientation\>
 * <br><i>normal</i> already has the correct orientation;
 * <br>if <i>orientation == -1</i> you should swap p1 with p3
 */
inline std::pair<glm::vec3, int> getNormal(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3) {
    glm::vec3 vector1 = p1 - p2;
    glm::vec3 vector2 = p2 - p3;

    glm::vec3 n = glm::normalize(glm::cross(vector1, vector2));
    glm::vec3 o = (p1 + p2 + p3) / 3.0f; // orientation
    glm::vec3 nxo = glm::cross(n, o);

    // https://stackoverflow.com/questions/5188561/signed-angle-between-two-3d-vectors-with-same-origin-within-the-same-plane

    auto angle = atan2(glm::dot(nxo, glm::normalize(nxo)), glm::dot(n, o));

    // fix vertex order if needed
    if (!((angle >= 0 && angle <= M_PI / 2) || (angle <= 0 && angle >= -M_PI / 2))) {
        n *= -1;
        return {n, -1};
    }

    return {n, 1};
}

inline glm::vec2 mapScreenToWorld(const glm::mat4 &projection, glm::vec2 screenPos, float z) {
    glm::mat4 invProjection = glm::inverse(projection);
    glm::vec4 screen {screenPos, 0.0f, 1.0f};

    glm::vec4 world = invProjection * screen;
    world /= world.w;
    world /= world.z;
    world *= z;

    return world;
}

#endif //ASTROVENTURE_VECTOOLS_H