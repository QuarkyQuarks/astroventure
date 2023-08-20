#include "IcoSphere.h"

#include <glm/geometric.hpp>
#include <unordered_map>

// icosahedron data
#define X .525731112119133606f
#define Z .850650808352039932f

static glm::vec3 initVertices[12] = {
    {-X, 0.f, Z}, {X, 0.f, Z}, {-X, 0.f, -Z}, {X, 0.f, -Z},
    {0.f, Z, X}, {0.f, Z, -X}, {0.f, -Z, X}, {0.f, -Z, -X},
    {Z, X, 0.0}, {-Z, X, 0.f}, {Z, -X, 0.f}, {-Z, -X, 0.f}
};

static Triangle initTriangles[20] = {
    {0, 4, 1}, {0, 9, 4}, {9, 5, 4}, {4, 5, 8}, {4, 8, 1},
    {8, 10, 1}, {8, 3, 10}, {5, 3, 8}, {5, 2, 3}, {2, 7, 3},
    {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6}, {0, 1, 6},
    {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5}, {7, 2, 11}
};

IcoSphere::IcoSphere(int subdivisions) {
    gen(subdivisions);
}

void IcoSphere::gen(int subdivisions) {
    vertices.reserve(12 * 3);
    trianglesCount = 20 * (4 << (2 * subdivisions - 1)); // 20 - initial count

    for (auto &v : initVertices) {
        vertices.emplace_back(v.x);
        vertices.emplace_back(v.y);
        vertices.emplace_back(v.z);
    }

    for (auto &tri : initTriangles)
        triangles.emplace_front(tri);

    for (int i = 0; i < subdivisions; ++i) {
        subdivide();
    }
}

void IcoSphere::subdivide() {
    struct Edge {
        int v1;
        int v2;

        bool operator==(const Edge &other) const {
            return (v1 == other.v1 && v2 == other.v2) || (v1 == other.v2 && v2 == other.v1);
        }
    };

    struct EdgeHash {
        std::size_t operator() (const Edge &edge) const {
            return std::hash<int>()(edge.v1) ^ std::hash<int>()(edge.v2);
        }
    };

    using Lookup = std::unordered_map<Edge, int, EdgeHash>; // Edge, vertex index

    Lookup lookupTable;
    Triangles sdTriangles; // subdivided triangles

    auto divide = [&](int v1, int v2) {
        // check if we have already divided this edge
        if (auto it = lookupTable.find({v1, v2}); it != lookupTable.end())
            return it->second;

        // if not - divide
        int vertexIndex = static_cast<int>(vertices.size() / 3);
        lookupTable[{v1, v2}] = vertexIndex;

        glm::vec3 a = {
            vertices[v1 * 3 + 0],
            vertices[v1 * 3 + 1],
            vertices[v1 * 3 + 2]
        };

        glm::vec3 b = {
            vertices[v2 * 3 + 0],
            vertices[v2 * 3 + 1],
            vertices[v2 * 3 + 2]
        };

        glm::vec3 c = glm::normalize((a + b) / 2.0f);

        vertices.emplace_back(c.x);
        vertices.emplace_back(c.y);
        vertices.emplace_back(c.z);

        return vertexIndex;
    };

    /*
     *          v1
     *          /\
     *        /    \
     *   d1 /--------\ d2
     *    /  \      /  \
     * v2 -------------- v3
     *          d3
     */

    for (auto [v1, v2, v3] : triangles) {
        int d1 = divide(v1, v2);
        int d2 = divide(v1, v3);
        int d3 = divide(v2, v3);

        sdTriangles.emplace_front(v1, d1, d2);
        sdTriangles.emplace_front(d1, v2, d3);
        sdTriangles.emplace_front(d1, d3, d2);
        sdTriangles.emplace_front(d2, d3, v3);
    }

    std::swap(triangles, sdTriangles);
}

glm::vec3& IcoSphere::getVertex(int v) {
    return *reinterpret_cast<glm::vec3*>(&vertices[v * 3 + 0]);
}
