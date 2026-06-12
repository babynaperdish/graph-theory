#ifndef GRAPHANALYZER_H
#define GRAPHANALYZER_H

#include "Graph.h"
#include <vector>
#include <utility>
#include <queue>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <limits>

class GraphAnalyzer
{
private:
    const Graph& graph;
    int n;
    std::vector<std::vector<int>> distances;
    Graph::GraphType type;

    void computeAllDistances();
    void computeAllDistancesDirected();

public:
    GraphAnalyzer(const Graph& g);

    std::vector<int> computeEccentricities() const;
    std::pair<int, std::vector<int>> findCenter() const;
    std::pair<int, std::vector<int>> findDiameter() const;

    void printResults() const;
    std::vector<int> findDiameterPath() const;

    std::vector<int> getShortestPath(int start, int end) const;

    bool isEulerian() const;
    bool isConnected() const;
    std::vector<int> findEulerianCycle() const;
    std::string solveEulerian();
};

#endif // GRAPHANALYZER_H
