#include "MaxFlow.h"
#include <algorithm>
#include <climits>

const int INF = 1e9;

MaxFlowGraph::MaxFlowGraph(int vertices)
    : n(vertices)
    , lastFordFulkersonIterations(0)
    , lastMinCostFlowIterations(0)
{
    capacity.resize(n, std::vector<int>(n, 0));
    cost.resize(n, std::vector<int>(n, 0));
    flow.resize(n, std::vector<int>(n, 0));
}

void MaxFlowGraph::generateCapacitiesAndCosts(const Graph& graph,
                                               LogDistribution& logDist,
                                               RandomGenerator& rng)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            capacity[i][j] = 0;
            cost[i][j] = 0;
            flow[i][j] = 0;
        }
    }
    if (graph.getType() == Graph::UNDIRECTED) {
        for (int u = 0; u < n; u++) {
            for (int v : graph.getNeighbors(u)) {
                if (u < v) {
                    int cap = logDist.generate() % 30 + 1;
                    if (cap < 1) cap = 1;
                    int cst = std::abs(logDist.generate() % 20);
                    capacity[u][v] = cap;
                    capacity[v][u] = cap;  // симметрично
                    cost[u][v] = cst;
                    cost[v][u] = cst;      // симметрично
                }
            }
        }
    } else {
        for (int u = 0; u < n; u++) {
            for (int v : graph.getNeighbors(u)) {
                int cap = logDist.generate() % 30 + 1;
                if (cap < 1) cap = 1;
                int cst = std::abs(logDist.generate() % 20);
                capacity[u][v] = cap;
                cost[u][v] = cst;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// алгоритм форда-фалкерсона
// -----------------------------------------------------------------------------

bool MaxFlowGraph::findAugmentingPath(int s, int t,
                                       std::vector<NodeLabel>& labels,
                                       int& augmentDelta)
{
    std::vector<int> S(n, 0);//достижимость вершины из s
    std::vector<int> N(n, 0);//просмотренность вершины
    labels.assign(n, NodeLabel{1, -1, 0});//метки: направление, предыдущая вершина, велчина изменения потока

    S[s] = 1;
    labels[s] = {1, -1, INF};//тк s принадлежит S

    while (true) {
        int a = 0;//признак расширения S

        for (int v = 0; v < n; v++) {
            if (S[v] == 1 && N[v] == 0) {
                for (int u = 0; u < n; u++) {//прями поток
                    if (S[u] == 0 && flow[v][u] < capacity[v][u]) {
                        S[u] = 1;
                        int newDelta = std::min(labels[v].delta,
                                                capacity[v][u] - flow[v][u]);
                        lastFordFulkersonIterations++;
                        labels[u] = {1, v, newDelta};
                        a = 1;//признак расширения
                    }
                }
                for (int u = 0; u < n; u++) {//обратный поток
                    if (S[u] == 0 && flow[u][v] > 0) {
                        S[u] = 1;
                        int newDelta = std::min(labels[v].delta, flow[u][v]);
                        labels[u] = {-1, v, newDelta};
                        a = 1;//признак расширения
                        lastFordFulkersonIterations++;
                    }
                }
                N[v] = 1;
            }
        }

        if (S[t] == 1) {
            augmentDelta = labels[t].delta;
            return true;
        }

        if (a == 0) {//
            break;
        }
    }
    return false;
}

void MaxFlowGraph::computeMaxFlow(int s, int t)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            flow[i][j] = 0;//изначально поток нулевой
        }
    }

    lastFordFulkersonIterations = 0;

    while (true) {
        std::vector<NodeLabel> labels(n);//знак, предыдущая вершина, максимальный поток от истока до этой вершины
        int delta = 0;

        if (!findAugmentingPath(s, t, labels, delta)) {//итерация увеличения потока
            break;
        }


        int x = t;//текущий узел аугментально цепи
        while (x != s) {
            lastFordFulkersonIterations++;
            if (labels[x].sign == 1) {
                flow[labels[x].from][x] += delta;//увеличение потока
            } else {
                flow[x][labels[x].from] -= delta;//увеличение потока
            }
            x = labels[x].from;//предыдущий узел аугментальной цепи
        }
    }
}

int MaxFlowGraph::fordFulkerson(int source, int sink)
{
    computeMaxFlow(source, sink);
    int maxFlow = 0;
    for (int v = 0; v < n; v++) {
        maxFlow += flow[source][v];
    }
    return maxFlow;
}

// -----------------------------------------------------------------------------
// поток мин стоимости
// -----------------------------------------------------------------------------

int MaxFlowGraph::minCostFlow(int source, int sink, int requiredFlow)
{
    std::cout << "=== minCostFlow START ===" << std::endl;
    std::cout << "n = " << n << ", source = " << source << ", sink = " << sink << ", requiredFlow = " << requiredFlow << std::endl;

    lastMinCostFlowIterations = 0;

    bool hasNegative = false;
    for (int i = 0; i < n && !hasNegative; i++) {
        for (int j = 0; j < n; j++) {
            if (cost[i][j] < 0) {
                hasNegative = true;
                break;
            }
        }
    }

    if (hasNegative) {
        std::cout << "WARNING: Negative costs detected! Converting to positive..." << std::endl;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (cost[i][j] < 0) {
                    cost[i][j] = -cost[i][j];
                }
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            flow[i][j] = 0;
        }
    }

    //временный граф для Дейкстры
    WeightedGraph tempGraph(n);

    int totalCost = 0;
    int currentFlow = 0;

    while (currentFlow < requiredFlow) {
        lastMinCostFlowIterations++;

        // заполнение временного графа остаточными стоимостями
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                int residualCap = capacity[i][j] - flow[i][j];
                if (residualCap > 0) {
                    tempGraph.setWeight(i, j, cost[i][j]);
                } else {
                    tempGraph.setWeight(i, j, WeightedGraph::getInfinity());
                }
            }
        }

        auto result = tempGraph.dijkstra(source, sink);

        if (result.distances[sink] >= WeightedGraph::getInfinity() - 1) {
            std::cout << "No augmenting path found! Current flow: " << currentFlow
                      << ", Required: " << requiredFlow << std::endl;
            break;
        }

        //восстановление пути
        std::vector<int> path;
        int current = sink;
        while (current != -1) {
            path.push_back(current);
            current = result.previous[current];
        }
        std::reverse(path.begin(), path.end());

        //нахождение макс потока на этом пути
        int pathFlow = requiredFlow - currentFlow;
        for (size_t i = 0; i < path.size() - 1; i++) {
            int u = path[i];
            int v = path[i + 1];
            pathFlow = std::min(pathFlow, capacity[u][v] - flow[u][v]);
        }

        //обновление потока и стоимости
        for (size_t i = 0; i < path.size() - 1; i++) {
            int u = path[i];
            int v = path[i + 1];
            flow[u][v] += pathFlow;
            flow[v][u] -= pathFlow;
            totalCost += pathFlow * cost[u][v];
        }

        currentFlow += pathFlow;
    }
    std::cout << "=== minCostFlow END ===" << std::endl;
    return totalCost;
}

std::string MaxFlowGraph::formatMaxFlowResult(int source, int sink) const
{
    std::string result;

    result += "╔══════════════════════════════════════════════════════════════════════════════╗\n";
    result += "║                    FORD-FULKERSON MAXIMUM FLOW ALGORITHM                     ║\n";
    result += "╚══════════════════════════════════════════════════════════════════════════════╝\n\n";

    result += "Source vertex (s): " + std::to_string(source) + "\n";
    result += "Sink vertex (t): " + std::to_string(sink) + "\n";
    result += "Total vertices: " + std::to_string(n) + "\n\n";

    MaxFlowGraph* nonConstThis = const_cast<MaxFlowGraph*>(this);
    int maxFlow = nonConstThis->fordFulkerson(source, sink);

    result += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    result += "RESULT:\n";
    result += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    result += "Max flow f(s,t) = " + std::to_string(maxFlow) + "\n\n";

    result += "By the Ford-Fulkerson theorem:\n";
    result += "  The value of the maximum flow equals the capacity of the minimum cut\n\n";

    result += "Statistics:\n";
    result += "  • Algorithm iterations: " + std::to_string(lastFordFulkersonIterations) + "\n";
    result += "  • Theoretical complexity: O(E × f_max)\n\n";


    return result;
}

std::string MaxFlowGraph::formatMinCostFlowResult(int source, int sink) const
{
    std::string result;

    result += "╔══════════════════════════════════════════════════════════════════════════════╗\n";
    result += "║                    MINIMUM COST FLOW ALGORITHM                               ║\n";
    result += "╚══════════════════════════════════════════════════════════════════════════════╝\n\n";

    result += "Source vertex (s): " + std::to_string(source) + "\n";
    result += "Sink vertex (t): " + std::to_string(sink) + "\n";
    result += "Total vertices: " + std::to_string(n) + "\n\n";

    MaxFlowGraph* nonConstThis = const_cast<MaxFlowGraph*>(this);
    int maxFlow = nonConstThis->fordFulkerson(source, sink);
    int requiredFlow = (maxFlow * 2) / 3;

    result += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    result += "FLOW PARAMETRS:\n";
    result += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    result += "  • Max flow: " + std::to_string(maxFlow) + "\n";
    result += "  • Required flow (2/3 × max): " + std::to_string(requiredFlow) + "\n\n";

    if (requiredFlow == 0) {
        result += "  ! Warning: Required flow is 0.\n\n";
        return result;
    }

    int minCost = nonConstThis->minCostFlow(source, sink, requiredFlow);

    result += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    result += "RESULT:\n";
    result += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    result += "Min cost for flow of size " + std::to_string(requiredFlow) + ": " + std::to_string(minCost) + "\n\n";

    result += "Algorithm used: Successive Shortest Path Algorithm\n";

    result += "Statistics:\n";
    result += "  • Algorithm iterations: " + std::to_string(lastMinCostFlowIterations) + "\n";
    result += "  • Theoretical complexity: O(flow x V^2)\n\n";

    return result;
}

void MaxFlowGraph::printMatrices() const
{
    std::cout << "\n=== CAPACITY MATRIX C(u,v) ===\n";
    std::cout << "     ";
    for (int j = 0; j < n; j++) {
        std::cout << j << "  ";
    }
    std::cout << "\n     ";
    for (int j = 0; j < n; j++) {
        std::cout << "---";
    }
    std::cout << "\n";

    for (int i = 0; i < n; i++) {
        std::cout << "  " << i << " | ";
        for (int j = 0; j < n; j++) {
            if (capacity[i][j] > 0) {
                std::cout << capacity[i][j] << "  ";
            } else {
                std::cout << "0  ";
            }
        }
        std::cout << "\n";
    }

    std::cout << "\n=== COST MATRIX W(u,v) ===\n";
    std::cout << "     ";
    for (int j = 0; j < n; j++) {
        std::cout << j << "  ";
    }
    std::cout << "\n     ";
    for (int j = 0; j < n; j++) {
        std::cout << "---";
    }
    std::cout << "\n";

    for (int i = 0; i < n; i++) {
        std::cout << "  " << i << " | ";
        for (int j = 0; j < n; j++) {
            if (cost[i][j] > 0) {
                std::cout << cost[i][j] << "  ";
            } else {
                std::cout << "0  ";
            }
        }
        std::cout << "\n";
    }
}
