#ifndef MAXFLOW_H
#define MAXFLOW_H

#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <iostream>
#include <string>
#include "Graph.h"
#include "RandomGenerator.h"
#include "LogDistribution.h"
#include "weightedgraph.h"

struct NodeLabel {
    int sign;      // +1 для прямой дуги, -1 для обратной
    int from;      // предыдущая вершина
    int delta;     // величина изменения потока
};

class MaxFlowGraph
{
private:
    int n;                                          // количество вершин
    std::vector<std::vector<int>> capacity;        // матрица пропускных способностей C
    std::vector<std::vector<int>> cost;            // матрица стоимости
    std::vector<std::vector<int>> flow;            // матрица потока F
    mutable int lastFordFulkersonIterations;
    mutable int lastMinCostFlowIterations;
    static const int INF = 1e9;
    std::vector<int> lastMinCostPath;

    // алгоритм расстановки пометок
    bool findAugmentingPath(int s, int t,
                            std::vector<NodeLabel>& labels,
                            int& augmentDelta);

    void computeMaxFlow(int s, int t);

public:
    MaxFlowGraph(int vertices);

    // генерация пропускных способностей и стоимостей на основе ориентированного графа
    void generateCapacitiesAndCosts(const Graph& graph,
                                    LogDistribution& logDist,
                                    RandomGenerator& rng);

    // алгоритм форда-фалкерсона (максимальный поток)
    int fordFulkerson(int source, int sink);

    // Алгоритм потока минимальной стоимости
    int minCostFlow(int source, int sink, int requiredFlow);

    // Геттеры
    const std::vector<std::vector<int>>& getCapacityMatrix() const { return capacity; }
    const std::vector<std::vector<int>>& getCostMatrix() const { return cost; }
    const std::vector<std::vector<int>>& getFlowMatrix() const { return flow; }

    int getLastFordFulkersonIterations() const { return lastFordFulkersonIterations; }
    int getLastMinCostFlowIterations() const { return lastMinCostFlowIterations; }

    // форматирование вывода
    std::string formatMaxFlowResult(int source, int sink) const;
    std::string formatMinCostFlowResult(int source, int sink) const;
    void printMatrices() const;

    static int getInfinity() { return INF; }
};

#endif // MAXFLOW_H
