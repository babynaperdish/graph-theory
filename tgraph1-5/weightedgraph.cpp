#include "WeightedGraph.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <set>
#include <iostream>
#include <iomanip>
#include <limits>

const double WeightedGraph::INF = 1e9;

void findAllSimplePathsRecursiveWithCounter(
    const std::vector<std::vector<double>>& weightMatrix,
    int start, int current, int target, int length,
    int remainingLength, std::vector<bool>& visited,
    std::vector<int>& currentPath, double currentWeight,
    std::vector<WeightedGraph::PathInfo>& results,
    bool allowCycles, int& iterations)//поиск путютютютей с итератором
{
    iterations++;

    if (remainingLength == 0) {
        if (current == target) {
            results.push_back(WeightedGraph::PathInfo(currentWeight, currentPath));
        }
        return;
    }

    if (remainingLength < 0) return;

    bool isLastStep = (remainingLength == 1);
    bool canVisitStart = allowCycles && isLastStep && (target == start);

    if (!canVisitStart) {
        visited[current] = true;
    }

    for (int next = 0; next < (int)weightMatrix.size(); next++) {
        iterations++;
        if (weightMatrix[current][next] < WeightedGraph::getInfinity() - 1) {
            bool canGo = !visited[next];
            if (allowCycles && isLastStep && next == start && next != current) {
                canGo = true;
            }

            if (canGo) {
                currentPath.push_back(next);
                findAllSimplePathsRecursiveWithCounter(weightMatrix, start, next, target, length,
                                                      remainingLength - 1, visited, currentPath,
                                                      currentWeight + weightMatrix[current][next],
                                                      results, allowCycles, iterations);
                currentPath.pop_back();
            }
        }
    }

    if (!canVisitStart) {
        visited[current] = false;
    }
}

WeightedGraph::WeightedGraph(int vertices)
    : n(vertices), edgeCount(0)
    , lastDijkstraIterations(0)
    , lastMinPathsIterations(0)
    , lastMaxPathsIterations(0)
    , lastSimpleMinPathsIterations(0)
    , lastSimpleMaxPathsIterations(0)
{
    weightMatrix.resize(n, std::vector<double>(n, INF));
    for (int i = 0; i < n; i++)
    {
        weightMatrix[i][i] = 0;
    }
}

void WeightedGraph::generateWeightsFromGraph(const Graph& graph, LogDistribution& logDist,
                                             SignType signType, RandomGenerator& rng)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            weightMatrix[i][j] = INF;
        }
        weightMatrix[i][i] = 0;
    }
    edges.clear();
    edgeCount = 0;

    std::set<std::pair<int, int>> processedEdges;

    for (int u = 0; u < n; u++) {
        for (int v : graph.getNeighbors(u)) {
            if (graph.getType() == Graph::UNDIRECTED) {
                if (processedEdges.count({u, v}) > 0 || processedEdges.count({v, u}) > 0) {
                    continue;
                }
                processedEdges.insert({u, v});
            }

            int weight = logDist.generate();

            weight = std::abs(weight) % 100;
            if (weight == 0) weight = 1;

            switch (signType)
            {
                case POSITIVE:
                    weight = std::abs(weight);
                    break;
                case NEGATIVE:
                    weight = -std::abs(weight);
                    break;
                case MIXED:
                    if (rng.uniformInt(0, 1) == 0)
                        weight = std::abs(weight);
                    else
                        weight = -std::abs(weight);
                    break;
            }

            weightMatrix[u][v] = weight;
            edges.push_back(Edge(u, v, weight));
            edgeCount++;

            if (graph.getType() == Graph::UNDIRECTED) {
                weightMatrix[v][u] = weight;
            }
        }
    }
}

//метод шушшушушушушшимбелла
//жооооск возводим матрицы в степень
std::vector<std::vector<double>> WeightedGraph::findMinPaths(int length) const
{
    lastMinPathsIterations = 0;
    std::vector<std::vector<double>> dist(n, std::vector<double>(n, INF));

    for (int i = 0; i < n; i++) {
        dist[i][i] = 0;
    }

    if (length == 0) {
        return dist;
    }

    if (length == 1) {
        return weightMatrix;
    }

    auto prevDist = weightMatrix;

    for (int step = 2; step <= length; step++) {
        lastMinPathsIterations++;
        std::vector<std::vector<double>> newDist(n, std::vector<double>(n, INF));

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                for (int k = 0; k < n; k++) {
                    lastMinPathsIterations++;
                    if (prevDist[i][k] < INF - 1 && weightMatrix[k][j] < INF - 1) {
                        double candidate = prevDist[i][k] + weightMatrix[k][j];
                        if (candidate < newDist[i][j] - 1e-12) {
                            newDist[i][j] = candidate;
                        }
                    }
                }
            }
        }

        prevDist = newDist;
    }

    return prevDist;
}

//метод шушшушушушушшимбелла
std::vector<std::vector<double>> WeightedGraph::findMaxPaths(int length) const
{
    lastMaxPathsIterations = 0;
    std::vector<std::vector<double>> dist(n, std::vector<double>(n, -INF));

    for (int i = 0; i < n; i++) {
        dist[i][i] = 0;
    }

    if (length == 0) {
        return dist;
    }

    if (length == 1) {
        auto result = weightMatrix;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (result[i][j] >= INF - 1) {
                    result[i][j] = -INF;
                }
            }
        }
        return result;
    }

    auto prevDist = weightMatrix;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (prevDist[i][j] >= INF - 1) {
                prevDist[i][j] = -INF;
            }
        }
    }

    for (int step = 2; step <= length; step++) {
        lastMaxPathsIterations++;
        std::vector<std::vector<double>> newDist(n, std::vector<double>(n, -INF));

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                for (int k = 0; k < n; k++) {
                    lastMaxPathsIterations++;
                    if (prevDist[i][k] > -INF + 1 && weightMatrix[k][j] < INF - 1) {
                        double candidate = prevDist[i][k] + weightMatrix[k][j];
                        if (candidate > newDist[i][j] + 1e-12) {
                            newDist[i][j] = candidate;
                        }
                    }
                }
            }
        }

        prevDist = newDist;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (prevDist[i][j] <= -INF + 1) {
                prevDist[i][j] = INF;
            }
        }
    }

    return prevDist;
}

std::vector<WeightedGraph::PathInfo> WeightedGraph::findAllSimplePaths(int start, int end, int length, bool allowCycles) const
{
    std::vector<PathInfo> results;
    int iterations = 0;

    if (length < 0) return results;

    if (length == 0) {
        if (start == end) {
            results.push_back(PathInfo(0.0, {start}));
        }
        return results;
    }

    std::vector<bool> visited(n, false);
    std::vector<int> currentPath;
    currentPath.push_back(start);

    findAllSimplePathsRecursiveWithCounter(weightMatrix, start, start, end, length, length,
                                           visited, currentPath, 0.0, results, allowCycles, iterations);

    lastSimpleMinPathsIterations = iterations;
    lastSimpleMaxPathsIterations = iterations;

    return results;
}

std::vector<std::vector<double>> WeightedGraph::findSimpleMinPaths(int length) const
{
    lastSimpleMinPathsIterations = 0;
    std::vector<std::vector<double>> result(n, std::vector<double>(n, INF));

    if (length == 0) {
        for (int i = 0; i < n; i++) {
            result[i][i] = 0;
        }
        return result;
    }

    if (length == 1) {
        return weightMatrix;
    }

    for (int start = 0; start < n; start++) {
        for (int end = 0; end < n; end++) {
            lastSimpleMinPathsIterations++;
            if (start == end && length > 0) {
                auto paths = findAllSimplePaths(start, end, length, true);
                double minWeight = INF;

                for (const auto& path : paths) {
                    lastSimpleMinPathsIterations++;
                    if (path.vertices.size() > 1 && path.weight < minWeight) {
                        minWeight = path.weight;
                    }
                }

                if (minWeight < INF - 1) {
                    result[start][end] = minWeight;
                }
            } else if (start != end) {
                auto paths = findAllSimplePaths(start, end, length, false);
                double minWeight = INF;

                for (const auto& path : paths) {
                    lastSimpleMinPathsIterations++;
                    if (path.weight < minWeight) {
                        minWeight = path.weight;
                    }
                }

                if (minWeight < INF - 1) {
                    result[start][end] = minWeight;
                }
            }
        }
    }

    return result;
}

std::vector<std::vector<double>> WeightedGraph::findSimpleMaxPaths(int length) const
{
    lastSimpleMaxPathsIterations = 0;
    std::vector<std::vector<double>> result(n, std::vector<double>(n, -INF));

    if (length == 0) {
        for (int i = 0; i < n; i++) {
            result[i][i] = 0;
        }
        return result;
    }

    if (length == 1) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (weightMatrix[i][j] >= INF - 1) {
                    result[i][j] = -INF;
                } else {
                    result[i][j] = weightMatrix[i][j];
                }
            }
        }
        return result;
    }

    for (int start = 0; start < n; start++) {
        for (int end = 0; end < n; end++) {
            lastSimpleMaxPathsIterations++;
            if (start == end && length > 0) {
                auto paths = findAllSimplePaths(start, end, length, true);
                double maxWeight = -INF;

                for (const auto& path : paths) {
                    lastSimpleMaxPathsIterations++;
                    if (path.vertices.size() > 1 && path.weight > maxWeight) {
                        maxWeight = path.weight;
                    }
                }

                if (maxWeight > -INF + 1) {
                    result[start][end] = maxWeight;
                }
            } else if (start != end) {
                auto paths = findAllSimplePaths(start, end, length, false);
                double maxWeight = -INF;

                for (const auto& path : paths) {
                    lastSimpleMaxPathsIterations++;
                    if (path.weight > maxWeight) {
                        maxWeight = path.weight;
                    }
                }

                if (maxWeight > -INF + 1) {
                    result[start][end] = maxWeight;
                }
            } else {
                result[start][end] = 0;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (result[i][j] <= -INF + 1) {
                result[i][j] = INF;
            }
        }
    }

    return result;
}

//------------------------Lab 2----------------------------
WeightedGraph::DijkstraResult WeightedGraph::dijkstra(int start, int end) const
{
    bool hasNegativeWeights = false;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != j && weightMatrix[i][j] < -1e-9 && weightMatrix[i][j] < INF - 1) {
                hasNegativeWeights = true;
                break;
            }
        }
    }

    if (hasNegativeWeights) {
        std::cout << "ERROR: Dijkstra algorithm cannot handle negative weights!" << std::endl;
        std::cout << "Please generate weight matrix with POSITIVE signs." << std::endl;
        return DijkstraResult(std::vector<double>(n, INF), std::vector<int>(n, -1), 0);
    }

    lastDijkstraIterations = 0;
    std::vector<double> T(n, INF); //длины кратчайших путей
    std::vector<int> H(n, -1); //предшественники на кратчайшем пути
    std::vector<int> X(n, 0); //метки обработанных вершин
    H[start] = -1;
    T[start] = 0;
    X[start] = 1;

    int current = start;

    while (true) {
        lastDijkstraIterations++;  // считаем одну итерацию основного цикла

        // M: обновление пометок всех соседей
        for (int u = 0; u < n; u++) {
            lastDijkstraIterations++;
            if (weightMatrix[current][u] < INF - 1) {
                if (X[u] == 0 && T[u] > T[current] + weightMatrix[current][u]) {
                    T[u] = T[current] + weightMatrix[current][u];
                    H[u] = current;
                }
            }
        }

        // поиск ближайшей необработанной вершины
        double m = INF;
        int v = -1;

        for (int u = 0; u < n; u++) {
            lastDijkstraIterations++;
            if (X[u] == 0 && T[u] < m) {
                v = u;
                m = T[u];
            }
        }

        if (v == -1) {
            break; //ниче не нашли
        }

        if (v == end) {
            X[v] = 1;
            break;//дошли до конца
        }

        X[v] = 1;
        current = v;
    }

    return DijkstraResult(T, H, lastDijkstraIterations);
}

std::string WeightedGraph::formatDijkstraPath(int start, int end) const
{
    std::string result;

    auto res = dijkstra(start, end);

    result += "=== DIJKSTRA ALGORITHM ===\n";
    result += "Start vertex: " + std::to_string(start) + "\n";
    result += "End vertex: " + std::to_string(end) + "\n\n";

    result += "Distance vector T[v] (shortest path from " + std::to_string(start) + " to v):\n";
    result += "Vertex | Distance\n";
    result += "-------+---------\n";
    for (int i = 0; i < n; i++) {
        if (res.distances[i] >= INF - 1) {
            result += "  " + std::to_string(i) + "   |    —\n";
        } else {
            result += "  " + std::to_string(i) + "   |    " +
                      std::to_string(static_cast<int>(res.distances[i])) + "\n";
        }
    }

    result += "\nPredecessor vector H[v] (vertex preceding v on the shortest path):\n";
    result += "Vertex | Predecessor\n";
    result += "-------+-------------\n";
    for (int i = 0; i < n; i++) {
        if (res.previous[i] == -1) {
            result += "  " + std::to_string(i) + "   |     —\n";
        } else {
            result += "  " + std::to_string(i) + "   |     " + std::to_string(res.previous[i]) + "\n";
        }
    }

    result += "\nPath from " + std::to_string(start) + " to " + std::to_string(end) + ":\n";

    if (res.distances[end] >= INF - 1) {
        result += "No path exists!\n";
    } else {
        std::vector<int> path;
        int current = end;
        while (current != -1) {
            path.push_back(current);
            current = res.previous[current];
        }

        std::reverse(path.begin(), path.end());

        result += "Distance: " + std::to_string(static_cast<int>(res.distances[end])) + "\n";
        result += "Route: ";
        for (size_t i = 0; i < path.size(); i++) {
            result += std::to_string(path[i]);
            if (i < path.size() - 1) result += " → ";
        }
        result += "\n";

        int theoreticalOps = n * n;  // O(P^2) операций
        result += "\nAlgorithm iterations (outer loop): " + std::to_string(res.iterations) + "\n";
        result += "Theoretical complexity: O(P²) = " + std::to_string(n) + "² = " + std::to_string(theoreticalOps) + " operations\n";
        result += "Note: Each outer iteration processes all vertices (2P operations), so total ~ " +
                  std::to_string(res.iterations) + " outer iterations × " + std::to_string(2 * n) + " = " +
                  std::to_string(res.iterations * 2 * n) + " actual operations\n";
    }

    return result;
}

//---------------------------------------------------------------

void WeightedGraph::printMatrix(const std::vector<std::vector<double>>& mat, const std::string& title) const
{
    std::cout << "\n" << title << ":\n";
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "    ";
    for (int j = 0; j < n; j++)
    {
        std::cout << std::setw(8) << j;
    }
    std::cout << "\n";
    std::cout << "    ";
    for (int j = 0; j < n; j++)
    {
        std::cout << "--------";
    }
    std::cout << "\n";

    for (int i = 0; i < n; i++)
    {
        std::cout << std::setw(3) << i << " |";
        for (int j = 0; j < n; j++)
        {
            if (mat[i][j] >= INF - 1)
                std::cout << std::setw(8) << "—";
            else if (mat[i][j] <= -INF + 1)
                std::cout << std::setw(8) << "—";
            else
                std::cout << std::setw(8) << mat[i][j];
        }
        std::cout << "\n";
    }
}
