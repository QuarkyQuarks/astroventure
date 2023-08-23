#ifndef SPACEEXPLORER_ODE_H
#define SPACEEXPLORER_ODE_H

#include <cmath>

#include <glm/geometric.hpp>

#include "types.h"

namespace mechanics {
class ODE {
public:
    static inline num_t linear(num_t p, num_t h) {
        num_t k1 = p;
        num_t k2 = p * k1 * (h / 2);
        num_t k3 = p * k2 * (h / 2);
        num_t k4 = p * k3 * h;
        return rk4(k1, k2, k3, k4, h);
    }

    template<auto ptr, int L>
    static num_t gravity(const glm::vec<L, num_t> &v, num_t h, num_t massTimesG = 0.15) {
        num_t val = v.*ptr;
        num_t k1 = -massTimesG * std::pow(glm::length(v), -3) * val;
        num_t k2 = -massTimesG * std::pow(glm::length(v + num_t(k1 * h / 2)), -3) * (val + k1 * h / 2);
        num_t k3 = -massTimesG * std::pow(glm::length(v + num_t(k2 * h / 2)), -3) * (val + k2 * h / 2);
        num_t k4 = -massTimesG * std::pow(glm::length(v + num_t(k3 * h)), -3) * (val + k3 * h);
        return rk4(k1, k2, k3, k4, h);
    }

private:
    static inline num_t rk4(num_t k1, num_t k2, num_t k3, num_t k4, num_t h) {
        return num_t(1.0) / 6 * h * (k1 + 2 * k2 + 2 * k3 + k4);
    }
};
}

#endif //SPACEEXPLORER_ODE_H

