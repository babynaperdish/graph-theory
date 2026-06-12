#ifndef SKELETON_H
#define SKELETON_H

#include "Graph.h"
#include "WeightedGraph.h"
#include <vector>
#include <limits>
#include <algorithm>
#include <iostream>
#include <string>
#include <set>
#include <sstream>
#include <iomanip>
#include <queue>

class Skeleton
{
public:
    struct Edge {
        int from;
        int to;
        double weight;

        Edge(int f, int t, double w) : from(f), to(t), weight(w) {}

        bool operator<(const Edge& other) const {
            if (weight != other.weight) return weight < other.weight;
            if (from != other.from) return from < other.from;
            return to < other.to;
        }
    };

private:
    int n;
    std::vector<std::vector<int>> kirchhoff;        // матрица Кирхгофа
    std::vector<std::vector<double>> weightMatrix;   // матрица весов

    // Вспомогательные методы для определителя
    std::vector<std::vector<double>> getMinor(const std::vector<std::vector<int>>& matrix,
                                               int row, int col) const;
    double determinant(std::vector<std::vector<double>> matrix) const;

    // Алгоритм Прима (строго по псевдокоду)
    std::vector<Edge> primMST() const;

    // Код Прюфера (строго по псевдокоду)
    std::vector<int> encodePrufer(const std::vector<Edge>& tree) const;
    std::vector<int> encodePruferWithWeights(const std::vector<Edge>& tree,
                                              std::vector<int>& outWeights) const;
    std::vector<Edge> decodePrufer(const std::vector<int>& code,
                                   const std::vector<int>& weights, Edge* lastEdge) const;

    // Минимальное рёберное покрытие
    std::vector<Edge> minEdgeCoverOnTree(const std::vector<Edge>& tree) const;
    std::vector<Edge> minEdgeCoverOnGraph() const;

    // Паросочетание (венгерский алгоритм для небольших графов)
    std::vector<int> maxMatching() const;

    // Форматирование
    std::string formatEdges(const std::vector<Edge>& edges) const;
    std::string formatCode(const std::vector<int>& code) const;

    std::vector<Edge> mstEdges;

public:
    Skeleton(int vertices, const Graph* graph = nullptr);
    Skeleton(const Graph& graph, const WeightedGraph* wg = nullptr);

    int skeletonAmount(const Graph& graph);
    long long countSpanningTrees() const;
    const std::vector<std::vector<int>>& getKirchhoffMatrix() const { return kirchhoff; }

    void setWeightMatrix(const WeightedGraph& wg);

    // Основные методы для использования в интерфейсе
    std::string solvePrimMST();
    std::string solveEdgeCoverOnTree();
    std::string solveEdgeCoverOnGraph();
    std::string solveAll();

    struct Cut {
        std::vector<int> vertices;      // вершины в одной части разреза (V1)
        std::vector<int> vertices2;      // вершины в другой части разреза (V2)
        std::vector<std::pair<int,int>> edges;  // рёбра разреза (без весов!)
    };

    std::vector<Cut> getFundamentalCuts();           // фундаментальная система разрезов
    Cut symmetricDifference(const Cut& c1, const Cut& c2);  // симметрическая разность
    std::string formatCut(const Cut& cut) const;     // форматирование одного разреза
    std::string solveCuts();
    Cut symmetricDifferenceMultiple(const std::vector<int>& cutIndices);



};

#endif // SKELETON_H
