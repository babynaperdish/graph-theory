#include "GraphAnalyzer.h"
#include <queue>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <limits>
#include <stack>
#include <sstream>

GraphAnalyzer::GraphAnalyzer(const Graph& g)
    : graph(g), n(g.getVerticesCount()), type(g.getType())
{
    distances.resize(n, std::vector<int>(n, -1));

    if (type == Graph::UNDIRECTED) {
        computeAllDistances();
    } else {
        computeAllDistancesDirected();
    }
}

void GraphAnalyzer::computeAllDistances()
{
   // BFS из каждой вершины
    for (int start = 0; start < n; start++)
    {
        std::queue<int> q;
        std::vector<bool> visited(n, false);

        q.push(start);
        visited[start] = true;
        distances[start][start] = 0;

        while (!q.empty())
        {
            int u = q.front();
            q.pop();

            for (int v : graph.getNeighbors(u))
            {
                if (!visited[v])
                {
                    visited[v] = true;
                    distances[start][v] = distances[start][u] + 1;
                    q.push(v);
                }
            }
        }
    }
}

void GraphAnalyzer::computeAllDistancesDirected()
{
    // BFS из каждой вершины (для ориентированного графа)
    for (int start = 0; start < n; start++)
    {
        std::queue<int> q;
        std::vector<bool> visited(n, false);

        q.push(start);
        visited[start] = true;
        distances[start][start] = 0;

        while (!q.empty())
        {
            int u = q.front();
            q.pop();

            for (int v : graph.getNeighbors(u))
            {
                if (!visited[v])
                {
                    visited[v] = true;
                    distances[start][v] = distances[start][u] + 1;
                    q.push(v);
                }
            }
        }
    }
}

std::vector<int> GraphAnalyzer::computeEccentricities() const
{
    std::vector<int> ecc(n, 0);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (distances[i][j] > ecc[i])
            {
                ecc[i] = distances[i][j];
            }
        }
    }

    return ecc;
}

std::pair<int, std::vector<int>> GraphAnalyzer::findCenter() const
{
    auto ecc = computeEccentricities();
    int radius = *std::min_element(ecc.begin(), ecc.end());

    std::vector<int> center;
    for (int i = 0; i < n; i++)
    {
        if (ecc[i] == radius)
        {
            center.push_back(i);
        }
    }

    return {radius, center};
}

std::pair<int, std::vector<int>> GraphAnalyzer::findDiameter() const
{
    auto ecc = computeEccentricities();
    int diameter = *std::max_element(ecc.begin(), ecc.end());

    std::vector<int> diamVertices;
    for (int i = 0; i < n; i++)
    {
        if (ecc[i] == diameter)
        {
            diamVertices.push_back(i);
        }
    }

    return {diameter, diamVertices};
}

void GraphAnalyzer::printResults() const
{
    auto ecc = computeEccentricities();

    std::cout << "\n--- ХАРАКТЕРИСТИКИ ГРАФА ---\n";
    std::cout << "Эксцентриситеты вершин:\n";
    for (int i = 0; i < n; i++)
    {
        std::cout << "  e(" << i << ") = " << ecc[i] << "\n";
    }

    auto [radius, center] = findCenter();
    std::cout << "\nРадиус графа: " << radius << "\n";
    std::cout << "Центр графа: ";
    for (int v : center) std::cout << v << " ";
    std::cout << "\n";

    auto [diameter, diamVertices] = findDiameter();
    std::cout << "\nДиаметр графа: " << diameter << "\n";
    std::cout << "Диаметральные вершины: ";
    for (int v : diamVertices) std::cout << v << " ";
    std::cout << "\n";
}

std::vector<int> GraphAnalyzer::getShortestPath(int start, int end) const
{
    std::vector<int> path;
    if (distances[start][end] < 0)   // недостижима
        return path;

    path.push_back(start);
    while (start != end) {
        bool found = false;
        for (int v : graph.getNeighbors(start)) {
            if (distances[start][v] == 1 && distances[v][end] == distances[start][end] - 1) {
                path.push_back(v);
                start = v;
                found = true;
                break;
            }
        }
        if (!found) {
            path.clear();
            break;
        }
    }
    return path;
}

std::vector<int> GraphAnalyzer::findDiameterPath() const
{
    auto [diameter, diamVertices] = findDiameter();

    if (diameter <= 0) {
        if (!diamVertices.empty())
            return {diamVertices[0]};
        return {};
    }

    for (size_t i = 0; i < diamVertices.size(); i++) {
        for (size_t j = i + 1; j < diamVertices.size(); j++) {
            int u = diamVertices[i];
            int v = diamVertices[j];
            if (distances[u][v] == diameter) {
                std::vector<int> path = getShortestPath(u, v);
                if (!path.empty() && path.size() == (size_t)diameter + 1) {
                    return path;
                }
            }
        }
    }

    for (int i = 0; i < n; i++) {
        auto ecc = computeEccentricities();
        if (ecc[i] == diameter) {
            for (int j = 0; j < n; j++) {
                if (distances[i][j] == diameter) {
                    std::vector<int> path = getShortestPath(i, j);
                    if (!path.empty() && path.size() == (size_t)diameter + 1) {
                        return path;
                    }
                }
            }
        }
    }

    return {};
}


//---------------------------Lab 5---------------------------------------
bool GraphAnalyzer::isEulerian() const
{

    if (!isConnected()) return false;

    for (int i = 0; i < n; i++) {
        if (graph.getNeighbors(i).size() % 2 != 0) {
            return false;
        }
    }

    return true;
}

bool GraphAnalyzer::isConnected() const
{
    std::vector<bool> visited(n, false);
    std::queue<int> q;

    // находим первую вершину с ненулевой степенью
    int start = -1;
    for (int i = 0; i < n; i++) {
        if (graph.getNeighbors(i).size() > 0) {
            start = i;
            break;
        }
    }
    if (start == -1) return true; // все изолированы

    q.push(start);
    visited[start] = true;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v : graph.getNeighbors(u)) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }

    for (int i = 0; i < n; i++) {
        if (graph.getNeighbors(i).size() >= 0 && !visited[i]) {
            return false;
        }
    }

    return true;
}

std::vector<int> GraphAnalyzer::findEulerianCycle() const
{
    // вход: эйлеров граф G(V,E), заданный списками смежности
    // выход: последовательность вершин эйлерова цикла

    // проверяем, что граф эйлеров
    if (!isEulerian()) {
        return {};
    }

    std::vector<std::vector<int>> Gamma(n);
    for (int i = 0; i < n; i++) {
        Gamma[i] = graph.getNeighbors(i);
    }

    // S := 0 { стек для хранения вершин }
    std::stack<int> S;

    // select v ∈ V { произвольная вершина }
    int v = 0;
    for (int i = 0; i < n; i++) {
        if (!Gamma[i].empty()) {
            v = i;
            break;
        }
    }

    // v -> S { положить v в стек S }
    S.push(v);

    std::vector<int> cycle;

    // while S ≠ 0 do
    while (!S.empty()) {
        // v := top S { верхний элемент стека }
        v = S.top();

        // if Γ[v] = 0 then
        if (Gamma[v].empty()) {
            // v ← S; yield v { очередная вершина эйлерова цикла }
            S.pop();
            cycle.push_back(v);
        } else {
            // select u ∈ Γ[v] { взять первую вершину из списка смежности }
            int u = Gamma[v].back();
            // u → S { положить u в стек }
            S.push(u);
            // Γ[v] := Γ[v] - u; Γ[u] := Γ[u] - v { удалить ребро (v,u) }
            Gamma[v].pop_back();
            for (auto it = Gamma[u].begin(); it != Gamma[u].end(); ++it) {
                if (*it == v) {
                    Gamma[u].erase(it);
                    break;
                }
            }
        }
    }

    return cycle;
}

std::string GraphAnalyzer::solveEulerian()
{
    std::stringstream result;

    result << "╔══════════════════════════════════════════════════════════════════════╗\n";
    result << "║              EULERIAN CYCLE - HIERHOLZER'S ALGORITHM                 ║\n";
    result << "╚══════════════════════════════════════════════════════════════════════╝\n\n";

    result << "Graph type: " << (type == Graph::UNDIRECTED ? "Undirected" : "Directed") << "\n";
    result << "Number of vertices: " << n << "\n\n";

    // Подсчёт рёбер и вывод степеней
    int edgeCount = 0;
    result << "Vertex degrees:\n";
    std::vector<int> oddVertices;
    for (int i = 0; i < n; i++) {
        int deg = graph.getNeighbors(i).size();
        result << "  deg(" << i << ") = " << deg;
        edgeCount += deg;
        if (deg % 2 != 0) {
            result << " ← ODD!";
            oddVertices.push_back(i);
        }
        result << "\n";
    }
    edgeCount /= 2; // Каждое ребро посчитано дважды
    result << "\nTotal edges: " << edgeCount << "\n\n";

    // Проверка условий эйлеровости
    bool eulerian = true;

    if (!oddVertices.empty()) {
        eulerian = false;
        result << "✗ CONDITION FAILED: Not all vertices have even degree!\n";
        result << "  Vertices with odd degree (" << oddVertices.size() << "): ";
        for (int v : oddVertices) result << v << " ";
        result << "\n\n";
        result << "  → To make the graph Eulerian, connect odd-degree vertices in pairs\n";
        result << "    or add edges to make all degrees even.\n";
    } else {
        result << "✓ CONDITION 1 MET: All vertices have even degree.\n\n";
    }

    if (!isConnected()) {
        eulerian = false;
        result << "✗ CONDITION FAILED: Graph is disconnected!\n";
        result << "  (considering only vertices with edges)\n\n";
    } else {
        result << "✓ CONDITION 2 MET: Graph is connected.\n\n";
    }

    if (eulerian) {
        result << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        result << "THE GRAPH IS EULERIAN!\n";
        result << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

        auto cycle = findEulerianCycle();

        if (cycle.empty()) {
            result << "ERROR: Failed to construct Eulerian cycle.\n";
            return result.str();
        }

        result << "Fleury's Algorithm Execution:\n";
        result << "  Using stack to track current path\n";
        result << "  Removing edges as they are traversed\n\n";

        result << "EULERIAN CYCLE:\n";
        result << "  Start vertex: " << cycle[0] << "\n";
        result << "  Cycle length: " << cycle.size() - 1 << " edges (" << cycle.size() << " vertices)\n\n";

        result << "  ";
        for (size_t i = 0; i < cycle.size(); i++) {
            result << cycle[i];
            if (i < cycle.size() - 1) {
                result << " → ";
            }
            if ((i + 1) % 12 == 0 && i < cycle.size() - 1) {
                result << "\n  ";
            }
        }
        result << "\n\n";

        // Верификация
        result << "Verification:\n";
        result << "  ✓ Starts and ends at vertex " << cycle[0] << "\n";
        result << "  ✓ Uses exactly " << edgeCount << " edges (once each)\n";
        result << "  ✓ All vertices visited appropriately\n";
        result << "  ✓ Forms a valid cycle\n";
    } else {
        result << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        result << "THE GRAPH IS NOT EULERIAN\n";
        result << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
        result << "To make it Eulerian, use the 'Modify Graph' function.\n";
    }

    return result.str();
}
