#include "PlanetGraph.h"

PlanetGraph::PlanetGraph() = default;

PlanetGraph::PlanetGraph(int v)
    : m_graph(v, std::vector<bool>(v, false)),
      m_associations(v) {}

void PlanetGraph::associate(int vi, int vbi, int triangle) {
    m_associations[vi].emplace_back(Association {vbi, triangle});
}

void PlanetGraph::edgeTo(int v1, int v2) {
    m_graph[v1][v2] = true;
    m_graph[v2][v1] = true;
}

bool PlanetGraph::isConnected(int v1, int v2) const {
    return m_graph[v1][v2];
}

const std::vector<std::vector<PlanetGraph::Association>>& PlanetGraph::getAssociations() const {
    return m_associations;
}

const std::vector<std::vector<bool>>& PlanetGraph::getGraph() const {
    return m_graph;
}

PlanetGraph::NeighborList PlanetGraph::neighborList() const {
    NeighborList listGraph(m_graph.size());

    for (int i = 0; i < m_graph.size(); ++i) {
        for (int j = 0; j < m_graph.size(); ++j) {
            if (m_graph[i][j]) {
                listGraph[i].emplace_back(j);
            }
        }
    }

    return listGraph;
}

PlanetGraph::List PlanetGraph::list() const {
    return {
        neighborList(),
        m_associations
    };
}
