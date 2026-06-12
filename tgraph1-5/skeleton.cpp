#include "Skeleton.h"
#include "Graph.h"
#include "WeightedGraph.h"
#include <cmath>

// ========== конструкторы ==========

Skeleton::Skeleton(int vertices, const Graph* graph) : n(vertices)
{
    kirchhoff.resize(n, std::vector<int>(n, 0));
    weightMatrix.resize(n, std::vector<double>(n, WeightedGraph::getInfinity()));
    for (int i = 0; i < n; i++) weightMatrix[i][i] = 0;

    if (graph != nullptr) {
        kirchhoff = graph->getKirchhoffMatrix();
        n = graph->getVerticesCount();
    }
}

Skeleton::Skeleton(const Graph& graph, const WeightedGraph* wg)
    : n(graph.getVerticesCount())
{
    kirchhoff = graph.getKirchhoffMatrix();
    weightMatrix.resize(n, std::vector<double>(n, WeightedGraph::getInfinity()));
    for (int i = 0; i < n; i++) weightMatrix[i][i] = 0;

    if (wg != nullptr) {
        weightMatrix = wg->getWeightMatrix();
    }
}

void Skeleton::setWeightMatrix(const WeightedGraph& wg)
{
    weightMatrix = wg.getWeightMatrix();
    n = weightMatrix.size();
    kirchhoff.resize(n, std::vector<int>(n, 0));
}

int Skeleton::skeletonAmount(const Graph& graph)
{
    kirchhoff = graph.getKirchhoffMatrix();
    n = graph.getVerticesCount();
    return static_cast<int>(countSpanningTrees());
}

// ========== минооооор ==========

std::vector<std::vector<double>> Skeleton::getMinor(
    const std::vector<std::vector<int>>& matrix, int row, int col) const
{
    int size = matrix.size();
    std::vector<std::vector<double>> minor(size - 1, std::vector<double>(size - 1, 0));

    int minorRow = 0;
    for (int i = 0; i < size; i++) {
        if (i == row) continue;
        int minorCol = 0;
        for (int j = 0; j < size; j++) {
            if (j == col) continue;
            minor[minorRow][minorCol] = matrix[i][j];
            minorCol++;
        }
        minorRow++;
    }
    return minor;
}

//определитель методом гаусса
double Skeleton::determinant(std::vector<std::vector<double>> matrix) const
{
    int size = matrix.size();
    if (size == 0) return 1;
    if (size == 1) return matrix[0][0];

    double det = 1.0;

    //ищем строку с макс эл-м для того чтобы не делить на слишком маленькое чыисло
    for (int i = 0; i < size; i++) {
        int pivotRow = i;
        double maxVal = std::abs(matrix[i][i]);
        for (int k = i + 1; k < size; k++) {
            if (std::abs(matrix[k][i]) > maxVal) {
                maxVal = std::abs(matrix[k][i]);
                pivotRow = k;
            }
        }
        if (maxVal < 1e-9) return 0;

        if (pivotRow != i) {
            std::swap(matrix[i], matrix[pivotRow]);
            det *= -1;
        }
        //приводим к верхней треугольной
        for (int k = i + 1; k < size; k++) {
            double factor = matrix[k][i] / matrix[i][i];
            for (int j = i; j < size; j++) {
                matrix[k][j] -= factor * matrix[i][j];
            }
        }
        det *= matrix[i][i];
    }
    return det;
}

long long Skeleton::countSpanningTrees() const
{
    if (n <= 1) return 1;
    std::vector<std::vector<double>> minor = getMinor(kirchhoff, 0, 0);
    return static_cast<long long>(std::abs(determinant(minor)));
}

// ========== алгоритм Прима  ==========

std::vector<Skeleton::Edge> Skeleton::primMST() const
{
    // вход: граф G(V, E), заданный матрицей длин рёбер С
    int p = n;
    if (p == 0) return {};

    // выбираем произвольную u
    int u = 0;

    // S - множество вершин, включённых в кратчайший остов (метки)
    std::vector<bool> S(p, false);
    S[u] = true;
    // T - множество рёбер, включенных в кратчайший остов
    std::vector<Edge> T;
    // alpha[v] - ближайшая вершина остова к v
    std::vector<int> alpha(p, 0);
    // beta[v] - длина ребра от alpha[v] до v
    std::vector<double> beta(p, WeightedGraph::getInfinity());

    // инициализация: for v in V \ {u}
    for (int v = 0; v < p; v++) {
        if (v == u) continue;
        if (weightMatrix[u][v] < WeightedGraph::getInfinity() - 1) {
            alpha[v] = u;
            beta[v] = weightMatrix[u][v];
        } else {
            alpha[v] = -1;
            beta[v] = WeightedGraph::getInfinity();
        }
    }

    // основной цикл: for i from 1 to p-1
    for (int i = 1; i <= p - 1; i++) {
        // поиск ближайшей вершины к остову
        double x = WeightedGraph::getInfinity();
        int w = -1; // ближайшая вершина

        for (int v = 0; v < p; v++) {
            if (!S[v]) {  // v in V \ S
                if (beta[v] < x) {
                    w = v;
                    x = beta[v];
                }
            }
        }

        if (w == -1) break;  // граф несвязный

        // добавляем найденную вершину в остов
        S[w] = true;
        T.push_back(Edge(alpha[w], w, weightMatrix[alpha[w]][w]));

        // обновляем метки для соседей w
        for (int v = 0; v < p; v++) {
            if (weightMatrix[w][v] < WeightedGraph::getInfinity() - 1) {
                if (!S[v]) {  // v not in S
                    if (beta[v] > weightMatrix[v][w]) {
                        alpha[v] = w;
                        beta[v] = weightMatrix[v][w];
                    }
                }
            }
        }
    }
    // выход: множество Т рёбер кратчайшего остова
    return T;
}

// ========== код Прюфера  ==========


std::vector<int> Skeleton::encodePruferWithWeights(const std::vector<Edge>& tree,
                                                    std::vector<int>& outWeights) const
{
    std::vector<std::set<int>> Gamma(n); // список смежности в процессе кодирования
    std::vector<int> degree(n, 0); // список степеней в процессе кодирования
    std::vector<std::vector<int>> edgeWeight(n, std::vector<int>(n, 0)); // список весов в процессе кодирования

    for (const auto& e : tree) {
        Gamma[e.from].insert(e.to);
        Gamma[e.to].insert(e.from);
        degree[e.from]++;
        degree[e.to]++;
        edgeWeight[e.from][e.to] = (int)e.weight;
        edgeWeight[e.to][e.from] = (int)e.weight;
    }

    std::vector<int> A;  // код прюфера
    outWeights.clear();
    std::set<int> V; // множество еще не удаленных вершин в процессе кодирования
    for (int i = 0; i < n; i++) V.insert(i);

    // for i from 1 to p-1
    for (int i = 0; i < n - 2; i++) {
        int v = -1;
        for (int k : V) {
            if (degree[k] == 1) {
                v = k;
                break;
            }
        }

        // добавляем вершину, смежную с v
        int neighbor = *Gamma[v].begin();
        A.push_back(neighbor);
        outWeights.push_back(edgeWeight[v][neighbor]);

        // удаляем вершину v
        V.erase(v);
        Gamma[neighbor].erase(v);
        degree[v]--;
        degree[neighbor]--;
    }

    if (V.size() == 2) {
        int a = *V.begin();
        int b = *V.rbegin();
        outWeights.push_back(edgeWeight[a][b]);  // вес последнего ребра
    }

    return A;
}

std::vector<Skeleton::Edge> Skeleton::decodePrufer(const std::vector<int>& code, const std::vector<int>& weights, Edge* lastEdge = nullptr) const
{
    // вход: массив A[1..p] - код Прюфера
    // выход: множество рёбер E

    int p = code.size() + 2;
    std::vector<Edge> E; //множество ребер
    std::set<int> B; //множество вершин остова которые еще не были использованы
    for (int i = 0; i < p; i++) B.insert(i);

    for (size_t i = 0; i < code.size(); i++) {
        // v := min{k in B | forall j >= i (k != A[j])}
        int v = -1;
        for (int k : B) {
            bool found = false;
            for (size_t j = i; j < code.size(); j++) {
                if (code[j] == k) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                v = k; //значит это лист, супееер
                break;
            }
        }

        int weight = (i < weights.size()) ? weights[i] : 0;
        E.push_back(Edge(v, code[i], weight));
        B.erase(v);
    }

    // последнее ребро - оставшиеся две вершины
    if (B.size() == 2) {
        int u = *B.begin();
        int v = *B.rbegin();
        int lastWeight = (weights.size() > code.size()) ? weights.back() : 0;
        Edge last(u, v, lastWeight);
        E.push_back(last);

        if (lastEdge != nullptr) {
            *lastEdge = last;
        }
    }

    return E;
}

// ========== минимальное рёберное покрытие ==========

std::vector<Skeleton::Edge> Skeleton::minEdgeCoverOnTree(const std::vector<Edge>& tree) const
{
    // жадный алгоритмик для остовного дерева

    std::vector<std::set<int>> adj(n);
    std::vector<int> degree(n, 0);
    std::vector<std::vector<double>> edgeW(n, std::vector<double>(n, -1));

    // жесткое наполнение
    for (const auto& e : tree) {
        adj[e.from].insert(e.to);
        adj[e.to].insert(e.from);
        degree[e.from]++;
        degree[e.to]++;
        edgeW[e.from][e.to] = e.weight;
        edgeW[e.to][e.from] = e.weight;
    }

    std::vector<bool> covered(n, false);
    std::vector<bool> edgeInCover(n * n, false);
    std::vector<Edge> cover;

    // находим листья и родителя
    std::queue<int> leaves;
    for (int i = 0; i < n; i++) {
        if (degree[i] == 1) leaves.push(i);
    }

    while (!leaves.empty()) {
        int leaf = leaves.front();
        leaves.pop();

        if (degree[leaf] == 0) continue;

        int parent = -1;
        for (int v : adj[leaf]) {
            if (degree[v] > 0) {
                parent = v;
                break;
            }
        }

        if (parent != -1 && !covered[leaf]) {
            // покрываем ребрышко
            cover.push_back(Edge(leaf, parent, edgeW[leaf][parent]));
            covered[leaf] = true;
            covered[parent] = true;

            // удаляем
            adj[parent].erase(leaf);
            degree[parent]--;
            degree[leaf]--;

            if (degree[parent] == 1) leaves.push(parent);
        }
    }

    // проверка все ли покрыли
    for (int i = 0; i < n; i++) {
            if (!covered[i]) {
                double minWeight = WeightedGraph::getInfinity();
                int bestNeighbor = -1;

                for (int j = 0; j < n; j++) {
                    if (i != j && weightMatrix[i][j] < WeightedGraph::getInfinity() - 1) {
                        if (weightMatrix[i][j] < minWeight) {
                            minWeight = weightMatrix[i][j];
                            bestNeighbor = j;
                        }
                    }
                }

                if (bestNeighbor != -1) {
                    cover.push_back(Edge(i, bestNeighbor, minWeight));
                    covered[i] = true;
                    covered[bestNeighbor] = true;
                }
            }
        }


    return cover;
}

std::vector<Skeleton::Edge> Skeleton::minEdgeCoverOnGraph() const
{
    std::vector<int> match(n, -1); //ну метч на то и метч что метч
    std::vector<bool> used(n, false); //метки соединенных

    // собираем вершины и сортируем по степени
    std::vector<std::pair<int, int>> verticesByDegree; // {degree, vertex}
    for (int i = 0; i < n; i++) {
        int deg = 0;
        for (int j = 0; j < n; j++) {
            if (i != j && weightMatrix[i][j] < WeightedGraph::getInfinity() - 1) {
                deg++;
            }
        }
        verticesByDegree.push_back({deg, i});
    }
    std::sort(verticesByDegree.begin(), verticesByDegree.end());

    // паросочетание с приоритетами по степеням связи
    for (const auto& p : verticesByDegree) {
        int i = p.second;
        if (used[i]) continue;

        // накидываем соседей
        std::vector<std::pair<int, int>> freeNeighbors; // {degree, vertex}
        for (int j = 0; j < n; j++) {
            if (!used[j] && i != j && weightMatrix[i][j] < WeightedGraph::getInfinity() - 1) {
                int deg = 0;
                for (int k = 0; k < n; k++) {
                    if (k != j && weightMatrix[j][k] < WeightedGraph::getInfinity() - 1) {
                        deg++;
                    }
                }
                freeNeighbors.push_back({deg, j});
            }
        }

        if (!freeNeighbors.empty()) {
            // выбираем минимального соседа
            std::sort(freeNeighbors.begin(), freeNeighbors.end());
            int j = freeNeighbors[0].second;

            match[i] = j;
            match[j] = i;
            used[i] = true;
            used[j] = true;
        }
    }

    std::vector<Edge> cover;
    std::vector<bool> covered(n, false);

    //добавляем рёбра в покрытие
    for (int i = 0; i < n; i++) {
        if (match[i] != -1 && i < match[i]) {
            cover.push_back(Edge(i, match[i], weightMatrix[i][match[i]]));
            covered[i] = true;
            covered[match[i]] = true;
        }
    }

    // покрываем оставшиеся
    std::vector<std::pair<int, int>> uncovered; // {degree, vertex}
    for (int i = 0; i < n; i++) {
        if (!covered[i]) {
            int deg = 0;
            for (int j = 0; j < n; j++) {
                if (i != j && weightMatrix[i][j] < WeightedGraph::getInfinity() - 1) {
                    deg++;
                }
            }
            uncovered.push_back({deg, i});
        }
    }
    // приоритеты: мин степень -> мин вес
    std::sort(uncovered.begin(), uncovered.end());
    for (const auto& p : uncovered) {
        int i = p.second;
        if (!covered[i]) {
            double minWeight = WeightedGraph::getInfinity();
            int bestNeighbor = -1;

            std::vector<std::pair<int, std::pair<double, int>>> candidates;
            for (int j = 0; j < n; j++) {
                if (i != j && weightMatrix[i][j] < WeightedGraph::getInfinity() - 1) {
                    int deg = 0;
                    for (int k = 0; k < n; k++) {
                        if (k != j && weightMatrix[j][k] < WeightedGraph::getInfinity() - 1) {
                            deg++;
                        }
                    }
                    candidates.push_back({deg, {weightMatrix[i][j], j}});
                }
            }

            if (!candidates.empty()) {
                std::sort(candidates.begin(), candidates.end());
                bestNeighbor = candidates[0].second.second;
                minWeight = candidates[0].second.first;
                cover.push_back(Edge(i, bestNeighbor, minWeight));
                covered[i] = true;
            }
        }
    }

    return cover;
}


std::string Skeleton::formatEdges(const std::vector<Edge>& edges) const
{
    std::stringstream ss;
    for (const auto& e : edges) {
        ss << "  (" << e.from << " - " << e.to << ") weight: " << (int)e.weight << "\n";
    }
    return ss.str();
}

std::string Skeleton::formatCode(const std::vector<int>& code) const
{
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < code.size(); i++) {
        if (i > 0) ss << ", ";
        ss << code[i];
    }
    ss << "]";
    return ss.str();
}


std::string Skeleton::solvePrimMST()
{
    std::stringstream result;
    result << "\n"
              "╔══════════════════════════════════════════════════════════════════╗\n";
    result << "║        NUMBER OF SPANNING TREES (Kirchhoff's Theorem)            ║\n";
    result << "╚══════════════════════════════════════════════════════════════════╝\n\n";

    long long treeCount = countSpanningTrees();

    if (treeCount == 0) {
        result << "τ(G) = 0 — Graph is disconnected, no spanning trees exist.\n";
    } else if (treeCount == 1) {
        result << "τ(G) = 1 — The graph is a tree itself (only one spanning tree).\n";
    } else {
        result << "τ(G) = " << treeCount << " spanning trees\n";
    }

    result << "╔══════════════════════════════════════════════════════════════════╗\n";
    result << "║              PRIM'S MINIMUM SPANNING TREE                        ║\n";
    result << "╚══════════════════════════════════════════════════════════════════╝\n\n";

    if (n == 0) {
        result << "No weighted graph loaded!\n";
        return result.str();
    }

    auto mst = primMST();

    double totalWeight = 0;
    for (const auto& e : mst) totalWeight += e.weight;

    result << "MST Edges (" << mst.size() << " edges):\n";
    result << formatEdges(mst);
    result << "Total MST weight: " << (int)totalWeight << "\n\n";

    // Кодирование Прюфера
    std::vector<int> weights;
    auto code = encodePruferWithWeights(mst, weights);

    result << "╔══════════════════════════════════════════════════════════════════╗\n";
    result << "║              PRUFER CODE ENCODING                                ║\n";
    result << "╚══════════════════════════════════════════════════════════════════╝\n\n";

    result << "Prufer code: " << formatCode(code) << "\n";
    if (weights.size() > 1) {
        std::vector<int> weightsExceptLast(weights.begin(), weights.end() - 1);
        result << "Edge weights: " << formatCode(weightsExceptLast);
    } else {
        result << "Edge weights: []";
    }
    if (!weights.empty()) {
            result << "[" << weights.back() << "]\n\n";
    }
    // Декодирование
    Skeleton::Edge lastEdge(0, 0, 0);
    auto decoded = decodePrufer(code, weights, &lastEdge);

    double decodedWeight = 0;
    for (const auto& e : decoded) decodedWeight += e.weight;

    result << "╔══════════════════════════════════════════════════════════════════╗\n";
    result << "║              PRUFER CODE DECODING                                ║\n";
    result << "╚══════════════════════════════════════════════════════════════════╝\n\n";

    result << "Decoded edges (" << decoded.size() << " edges):\n";
    result << formatEdges(decoded);
    result << "Decoded total weight: " << (int)decodedWeight << "\n\n";


    return result.str();
}

std::string Skeleton::solveEdgeCoverOnTree()
{
    std::stringstream result;

    result << "╔══════════════════════════════════════════════════════════════════╗\n";
    result << "║        MINIMUM EDGE COVER ON MST (TREE)                          ║\n";
    result << "╚══════════════════════════════════════════════════════════════════╝\n\n";

    if (n == 0) {
        result << "No weighted graph loaded!\n";
        return result.str();
    }

    auto mst = primMST();
    auto cover = minEdgeCoverOnTree(mst);

    double totalWeight = 0;
    for (const auto& e : cover) totalWeight += e.weight;

    result << "Minimum Edge Cover on MST:\n";
    result << formatEdges(cover);
    result << "Number of edges: " << cover.size() << "\n";
    result << "Total cover weight: " << (int)totalWeight << "\n";
    result << "Vertices covered: " << std::min(n, (int)cover.size() * 2) << " / " << n << "\n";

    return result.str();
}

std::string Skeleton::solveEdgeCoverOnGraph()
{
    std::stringstream result;

    result << "╔══════════════════════════════════════════════════════════════════╗\n";
    result << "║        MINIMUM EDGE COVER ON ORIGINAL GRAPH                      ║\n";
    result << "╚══════════════════════════════════════════════════════════════════╝\n\n";

    if (n == 0) {
        result << "No weighted graph loaded!\n";
        return result.str();
    }

    auto cover = minEdgeCoverOnGraph();

    double totalWeight = 0;
    for (const auto& e : cover) totalWeight += e.weight;

    result << "Minimum Edge Cover on Original Graph:\n";
    result << formatEdges(cover);
    result << "Number of edges: " << cover.size() << "\n";
    result << "Total cover weight: " << (int)totalWeight << "\n";
    result << "Vertices covered: " << std::min(n, (int)cover.size() * 2) << " / " << n << "\n";

    return result.str();
}

std::string Skeleton::solveAll()
{
    std::string result;

    result += "╔══════════════════════════════════════════════════════════════════════════════╗\n";
    result += "║             TAB 6: MINIMUM SPANNING TREE & EDGE COVER                        ║\n";
    result += "╚══════════════════════════════════════════════════════════════════════════════╝\n\n";
    result += "Graph has " + std::to_string(n) + " vertices\n\n";

    result += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    result += solvePrimMST();
    result += "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    result += solveEdgeCoverOnTree();
    result += "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    result += solveEdgeCoverOnGraph();

    return result;
}

// ========== фундаментальная система разрезов ==========

std::vector<Skeleton::Cut> Skeleton::getFundamentalCuts()
{
    std::vector<Edge> mst = primMST();// остов
    mstEdges = mst;

    // список смежности остова
    std::vector<std::vector<int>> treeAdj(n);
    for (const auto& e : mst) {
        treeAdj[e.from].push_back(e.to);
        treeAdj[e.to].push_back(e.from);
    }

    std::vector<Cut> fundamentalCuts;

    for (const auto& e : mst) {
        int u = e.from;
        int v = e.to;

        // обходим дерево начиная с u
        std::vector<bool> visited(n, false);
        std::queue<int> q; // вершины связанные с u

        q.push(u);
        visited[u] = true;

        while (!q.empty()) {
            int cur = q.front();
            q.pop();

            for (int neighbor : treeAdj[cur]) {
                // пропускаем переход через ребро (u,v)
                if ((cur == u && neighbor == v) || (cur == v && neighbor == u)) {
                    continue;
                }
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }

        // visited[i] == true  → вершина в V1 (достижима из u без ребра (u,v))
        // visited[i] == false → вершина в V2

        Cut cut;

        // сохраняем вершины в V1 и V2
        for (int i = 0; i < n; i++) {
            if (visited[i]) {
                cut.vertices.push_back(i);// добавляем соединенные с u в v1
            }
            else cut.vertices2.push_back(i);// добавляем остальные в v2
        }

        // добавляем все рёбра между V1 и V2
        for (int i = 0; i < n; i++) {
            if (!visited[i]) continue;  // i in V1

            for (int j = 0; j < n; j++) {
                if (visited[j]) continue;  // j in V2

                if (weightMatrix[i][j] < WeightedGraph::getInfinity() - 1) {
                    int a = std::min(i, j);
                    int b = std::max(i, j);
                    cut.edges.push_back({a, b});
                }
            }
        }

        // удаляем дубликаты ребер
        std::vector<std::pair<int,int>> uniqueEdges;
        std::sort(cut.edges.begin(), cut.edges.end());
        for (const auto& edge : cut.edges) {
            if (uniqueEdges.empty() || uniqueEdges.back() != edge) {
                uniqueEdges.push_back(edge);
            }
        }
        cut.edges = uniqueEdges;

        fundamentalCuts.push_back(cut);
    }

    return fundamentalCuts;
}

Skeleton::Cut Skeleton::symmetricDifference(const Cut& c1, const Cut& c2)
{
    Cut result; //тут хранятся результаты нашей симм разницы

    // симметрическая разность множеств вершин V₁
    std::set<int> v1(c1.vertices.begin(), c1.vertices.end());
    std::set<int> v2(c2.vertices.begin(), c2.vertices.end());

    for (int v : c1.vertices) {
        if (v2.find(v) == v2.end()) result.vertices.push_back(v);
    }
    for (int v : c2.vertices) {
        if (v1.find(v) == v1.end()) result.vertices.push_back(v);
    }
    std::sort(result.vertices.begin(), result.vertices.end());

    // симметрическая разность рёбер
    std::set<std::pair<int,int>> edges1, edges2;
    for (const auto& e : c1.edges) {
        edges1.insert({std::min(e.first, e.second), std::max(e.first, e.second)});
    }
    for (const auto& e : c2.edges) {
        edges2.insert({std::min(e.first, e.second), std::max(e.first, e.second)});
    }

    for (const auto& e : edges1) {
        if (edges2.find(e) == edges2.end()) {
            result.edges.push_back(e);
        }
    }
    for (const auto& e : edges2) {
        if (edges1.find(e) == edges1.end()) {
            result.edges.push_back(e);
        }
    }

    return result;
}

Skeleton::Cut Skeleton::symmetricDifferenceMultiple(const std::vector<int>& cutIndices)
{
    auto allCuts = getFundamentalCuts();

    if (cutIndices.empty()) {
        return Cut();
    }

    // начинаем с первого разреза
    Cut result = allCuts[cutIndices[0]];

    // последовательно применяем симметрическую разность
    for (size_t i = 1; i < cutIndices.size(); i++) {
        result = symmetricDifference(result, allCuts[cutIndices[i]]);
    }

    return result;
}

std::string Skeleton::formatCut(const Cut& cut) const
{
    std::stringstream ss;

    ss << "V1 = {";
    for (size_t i = 0; i < cut.vertices.size(); i++) {
        ss << cut.vertices[i];
        if (i < cut.vertices.size() - 1) ss << ", ";
    }
    ss << "}\n";

    ss << "V2 = {";
    for (size_t i = 0; i < cut.vertices2.size(); i++) {
        ss << cut.vertices2[i];
        if (i < cut.vertices2.size() - 1) ss << ", ";
    }
    ss << "}\n";

    ss << "Edges in cut S (" << cut.edges.size() << " edges):\n";
    ss << "  {";
    for (size_t i = 0; i < cut.edges.size(); i++) {
        ss << "(" << cut.edges[i].first << ", " << cut.edges[i].second << ")";
        if (i < cut.edges.size() - 1) ss << ", ";
    }
    ss << "}\n";

    return ss.str();
}

std::string Skeleton::solveCuts()
{
    std::stringstream result;

    result << "╔════════════════════════════════════════════╗\n";
    result << "║        FUNDAMENTAL CUT SYSTEM              ║\n";
    result << "╚════════════════════════════════════════════╝\n\n";

    if (n == 0) {
        result << "No graph loaded!\n";
        return result.str();
    }

    auto mst = primMST();
    int coRank = mst.size();  // коциклический ранг

    result << "Minimum Spanning Tree T (" << coRank << " edges):\n";
    for (const auto& e : mst) {
        result << "  (" << e.from << ", " << e.to << ")\n";
    }
    result << "\n";

    result << "Cocyclomatic number m*(G) = " << coRank << "\n\n";

    auto fundamentalCuts = getFundamentalCuts();

    result << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    result << "FUNDAMENTAL CUT SYSTEM δ = {S_e}_{e∈T}\n";
    result << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    for (size_t i = 0; i < fundamentalCuts.size(); i++) {
        result << "S_" << i << " (removing edge " << i << " of T):\n";
        result << formatCut(fundamentalCuts[i]);
        result << "\n";
    }
    return result.str();
}
