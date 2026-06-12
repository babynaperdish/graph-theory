#ifndef WEIGHTEDGRAPH_H
#define WEIGHTEDGRAPH_H

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include "RandomGenerator.h"
#include "LogDistribution.h"
#include "Graph.h"

class WeightedGraph
{
public:
    enum SignType { POSITIVE, NEGATIVE, MIXED };

    struct Edge {
        int from;
        int to;
        double weight;

        Edge(int f, int t, double w) : from(f), to(t), weight(w) {}
    };

    struct PathInfo {
        double weight;
        std::vector<int> vertices;

        PathInfo() : weight(INF) {}
        PathInfo(double w) : weight(w) {}
        PathInfo(double w, const std::vector<int>& path) : weight(w), vertices(path) {}

        bool isValid() const { return weight < INF - 1 && weight > -INF + 1; }
    };
    //--------------------Lab 2----------------------------
    struct DijkstraResult {
        std::vector<double> distances;
        std::vector<int> previous;
        bool success;
        int iterations;

        DijkstraResult() : success(false), iterations(0) {}
        DijkstraResult(const std::vector<double>& d, const std::vector<int>& p, int iter)
            : distances(d), previous(p), success(true), iterations(iter) {}
    };
    //-------------------------------------------------------
private:
    int n;
    std::vector<std::vector<double>> weightMatrix;
    std::vector<Edge> edges;
    static const double INF;
    int edgeCount;


    //--------------------Lab 2----------------------------
    mutable int lastDijkstraIterations;
    mutable int lastMinPathsIterations;
    mutable int lastMaxPathsIterations;
    mutable int lastSimpleMinPathsIterations;
    mutable int lastSimpleMaxPathsIterations;
    //----------------------------------------------
public:
    WeightedGraph(int vertices);

    void generateWeightsFromGraph(const Graph& graph, LogDistribution& logDist,
                                  SignType signType, RandomGenerator& rng);

    const std::vector<std::vector<double>>& getWeightMatrix() const { return weightMatrix; }
    const std::vector<Edge>& getEdges() const { return edges; }

    std::vector<std::vector<double>> findMinPaths(int length) const;
    std::vector<std::vector<double>> findMaxPaths(int length) const;
    std::vector<std::vector<double>> findSimpleMinPaths(int length) const;
    std::vector<std::vector<double>> findSimpleMaxPaths(int length) const;

    std::vector<PathInfo> findAllSimplePaths(int start, int end, int length, bool allowCycles = true) const;

    DijkstraResult dijkstra(int start, int end) const;
    std::string formatDijkstraPath(int start, int end) const;

    void printMatrix(const std::vector<std::vector<double>>& mat, const std::string& title) const;

    int getEdgeCount() const { return edgeCount; }
    static double getInfinity() { return INF; }

    //--------------------Lab 2----------------------------
    int getLastDijkstraIterations() const { return lastDijkstraIterations; }
    int getLastMinPathsIterations() const { return lastMinPathsIterations; }
    int getLastMaxPathsIterations() const { return lastMaxPathsIterations; }
    int getLastSimpleMinPathsIterations() const { return lastSimpleMinPathsIterations; }
    int getLastSimpleMaxPathsIterations() const { return lastSimpleMaxPathsIterations; }
    //--------------------Lab 3------------------------------
    void setWeight(int from, int to, double weight) {
        if (from >= 0 && from < n && to >= 0 && to < n) {
            weightMatrix[from][to] = weight;
        }
    }
};

#endif
