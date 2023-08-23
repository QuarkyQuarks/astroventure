#ifndef ASTROVENTURE_PLANETGRAPH_H
#define ASTROVENTURE_PLANETGRAPH_H

#include <vector>

/*
 * Planet Graph is a VxV matrix, where V - number of vertices
 * if graph[v1][v2] is true, that means that v1 and v2 are connected,
 * i.e. exists edge from v1 to v2 (and vice versa, from v2 to v1)
 * Each vertex index (vn) has its own list of associated vertex
 * indices in vertex buffer. Moreover, each association is linked with
 * triangle index
 */
class PlanetGraph {
public:
    struct Association {
        int vb;
        int triangle; // TODO: do we need this?
    };

    using NeighborList = std::vector<std::vector<int>>;

    struct List {
        NeighborList graph; // graph list representation
        std::vector<std::vector<Association>> associations;
    };

public:
    PlanetGraph();
    explicit PlanetGraph(int v);

    void associate(int vi, int vbi, int triangle);
    void edgeTo(int v1, int v2);

    bool isConnected(int v1, int v2) const;

    const std::vector<std::vector<Association>>& getAssociations() const;
    const std::vector<std::vector<bool>>& getGraph() const;

    NeighborList neighborList() const;
    List list() const;

private:
    std::vector<std::vector<bool>> m_graph;
    std::vector<std::vector<Association>> m_associations;
};

#endif //ASTROVENTURE_PLANETGRAPH_H
