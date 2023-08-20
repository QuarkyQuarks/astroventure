#ifndef ASTROVENTURE_ICOSPHERE_H
#define ASTROVENTURE_ICOSPHERE_H

#include "Triangle.h"

#include <forward_list>
#include <vector>

#include <glm/vec3.hpp>

class IcoSphere {
public:
    IcoSphere() = default;
    explicit IcoSphere(int subdivisions);

    void gen(int subdivisions);

    glm::vec3& getVertex(int v);

public:
    using Triangles = std::forward_list<Triangle>;

    Triangles triangles;
    size_t trianglesCount {};
    std::vector<float> vertices;

private:
    void subdivide();
};

#endif //ASTROVENTURE_ICOSPHERE_H
