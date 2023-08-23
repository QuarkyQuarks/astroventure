#ifndef SPACEEXPLORER_TYPES_H
#define SPACEEXPLORER_TYPES_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace mechanics {
using num_t = double;

template<int L>
using vec = glm::vec<L, num_t, glm::precision::defaultp>;

using vec2 = vec<2>;
using vec3 = vec<3>;
}

#endif //SPACEEXPLORER_TYPES_H
