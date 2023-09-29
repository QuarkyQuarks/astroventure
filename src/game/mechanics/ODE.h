#ifndef ASTROVENTURE_ODE_H
#define ASTROVENTURE_ODE_H

#include <cmath>

#include <glm/geometric.hpp>

#include "types.h"

namespace mechanics {
class ODE {
public:
    static inline num_t linear(num_t p, num_t E) {
        num_t k1 = p;
        num_t k2 = p * k1 * (E / 2);
        num_t k3 = p * k2 * (E / 2);
        num_t k4 = p * k3 * E;
        return rk4(k1, k2, k3, k4, E);
    }

    template<int L>
    static num_t spherically_symmetric(
        const glm::vec<L, num_t> &r,
        const glm::vec<L, num_t> &component,
        num_t E,
        num_t g = 0.17,
        num_t n = 1
    ) {
        using T = glm::vec<L, num_t>;

        const num_t componentAbs = glm::dot(component, T(1.0));
        const T versor = component / componentAbs;

        // gradient of the spherically symmetric potential V = - g / r^n
        auto potentialForce = [=](const T &vector, num_t scalar) {
            return -g * std::pow(glm::length(vector), -(n + 2)) * scalar;
        };

        num_t k1 = potentialForce(r, componentAbs);
        num_t k2 = potentialForce(r + versor * (k1 * E / 2), componentAbs + k1 * E / 2);
        num_t k3 = potentialForce(r + versor * (k2 * E / 2), componentAbs + k2 * E / 2);
        num_t k4 = potentialForce(r + versor * (k3 * E), componentAbs + k3 * E);

        return rk4(k1, k2, k3, k4, E);
    }

private:
    static inline num_t rk4(num_t k1, num_t k2, num_t k3, num_t k4, num_t h) {
        return num_t(1.0) / 6 * h * (k1 + 2 * k2 + 2 * k3 + k4);
    }
};
}

#endif //ASTROVENTURE_ODE_H

