#include "Graph.h"
#include <queue>
#include <algorithm>
#include <random>
#include <iostream>
#include <iomanip>
#include <set>
#include <sstream>

Graph::Graph(int vertices, GraphType t)
    : n(vertices), type(t), dfsIterations(0)
{
    adj.resize(n);
    outDegrees.resize(n, 0);
    inDegrees.resize(n, 0);
}

bool Graph::buildGraphFromDesiredDegrees(const std::vector<int>& desiredDegrees) //построение неориентированного графа
{
    if (type == DIRECTED) {
        return false;
    }

    int n = desiredDegrees.size();

    std::vector<int> finalDegrees = desiredDegrees;

    // нормировочка
    int maxAllowed = n - 1;
    int maxDegree = 0;
    for (int d : finalDegrees) {
        if (d > maxDegree) maxDegree = d;
    }

    if (maxDegree > maxAllowed) {
        double coeff = (double)maxAllowed / maxDegree;
        for (int i = 0; i < n; i++) {
            finalDegrees[i] = (int)(finalDegrees[i] * coeff);
            if (finalDegrees[i] < 1) finalDegrees[i] = 1;
            if (finalDegrees[i] > maxAllowed) finalDegrees[i] = maxAllowed;
        }
    }

    int sum = 0;
    for (int d : finalDegrees) sum += d;

    if (sum % 2 != 0) {
        for (int i = 0; i < n; i++) {
            if (finalDegrees[i] < n - 1) {
                finalDegrees[i]++;
                sum++;
                break;
            }
        }
        if (sum % 2 != 0) {
            finalDegrees[0]--;
        }
    }

    degrees = finalDegrees;

    for (auto& neighbors : adj)
        neighbors.clear();

    std::vector<int> stubs;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < degrees[i]; j++) {
            stubs.push_back(i);
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(stubs.begin(), stubs.end(), g);

    std::set<std::pair<int, int>> edgesSet;

    for (size_t i = 0; i + 1 < stubs.size(); i += 2) {
        int u = stubs[i];
        int v = stubs[i + 1];

        if (u == v) continue;

        if (u > v) std::swap(u, v);

        if (edgesSet.find({u, v}) != edgesSet.end()) {
            continue;
        }

        adj[u].push_back(v);
        adj[v].push_back(u);
        edgesSet.insert({u, v});
    }

    std::uniform_int_distribution<int> dist(0, n - 1);

    while ((int)edgesSet.size() < sum / 2) {
        int u = dist(g);
        int v = dist(g);

        if (u == v) continue;
        if (u > v) std::swap(u, v);

        if (edgesSet.find({u, v}) == edgesSet.end()) {
            adj[u].push_back(v);
            adj[v].push_back(u);
            edgesSet.insert({u, v});
        }
    }

    if (!isConnected()) {
        std::vector<int> component(n, -1);
        int compCount = 0;

        for (int i = 0; i < n; i++) {
            if (component[i] == -1) {
                std::queue<int> q;
                q.push(i);
                component[i] = compCount;

                while (!q.empty()) {
                    int u = q.front();
                    q.pop();
                    for (int v : adj[u]) {
                        if (component[v] == -1) {
                            component[v] = compCount;
                            q.push(v);
                        }
                    }
                }
                compCount++;
            }
        }

        for (int c = 1; c < compCount; c++) {
            int u = -1, v = -1;
            for (int i = 0; i < n; i++) {
                if (component[i] == 0 && u == -1) u = i;
                if (component[i] == c && v == -1) v = i;
            }
            if (u != -1 && v != -1) {
                adj[u].push_back(v);
                adj[v].push_back(u);
            }
        }
    }

    for (int i = 0; i < n; i++) {
        degrees[i] = adj[i].size();
    }

    return true;
}

bool Graph::buildDirectedGraphFromDesiredDegrees(const std::vector<int>& desiredOutDegrees,
                                                  const std::vector<int>& desiredInDegrees) //построение ориентированного графа
{
    if (type != DIRECTED) return false;

    int n = desiredOutDegrees.size();

    std::vector<int> finalOutDegrees = desiredOutDegrees;
    std::vector<int> finalInDegrees = desiredInDegrees;

    // нормировочка
    int maxAllowed = n - 1;
    int maxOut = 0, maxIn = 0;
    for (int d : finalOutDegrees) if (d > maxOut) maxOut = d;
    for (int d : finalInDegrees) if (d > maxIn) maxIn = d;

    if (maxOut > maxAllowed) {
        double coeff = (double)maxAllowed / maxOut;
        for (int i = 0; i < n; i++) {
            finalOutDegrees[i] = (int)(finalOutDegrees[i] * coeff);
            if (finalOutDegrees[i] < 1) finalOutDegrees[i] = 1;
            if (finalOutDegrees[i] > maxAllowed) finalOutDegrees[i] = maxAllowed;
        }
    }

    if (maxIn > maxAllowed) {
        double coeff = (double)maxAllowed / maxIn;
        for (int i = 0; i < n; i++) {
            finalInDegrees[i] = (int)(finalInDegrees[i] * coeff);
            if (finalInDegrees[i] < 1) finalInDegrees[i] = 1;
            if (finalInDegrees[i] > maxAllowed) finalInDegrees[i] = maxAllowed;
        }
    }

    for (int i = 0; i < n; i++) {
        if (finalOutDegrees[i] < 1) finalOutDegrees[i] = 1;
        if (finalInDegrees[i] < 1) finalInDegrees[i] = 1;
        if (finalOutDegrees[i] > n - 1) finalOutDegrees[i] = n - 1;
        if (finalInDegrees[i] > n - 1) finalInDegrees[i] = n - 1;
    }

    int outSum = 0, inSum = 0;
    for (int d : finalOutDegrees) outSum += d;
    for (int d : finalInDegrees) inSum += d;

    if (outSum != inSum) {
        int diff = outSum - inSum;
        if (diff > 0) {
            finalInDegrees[0] += diff;
        } else {
            finalOutDegrees[0] += -diff;
        }
    }

    outDegrees = finalOutDegrees;
    inDegrees = finalInDegrees;

    for (auto& neighbors : adj)
        neighbors.clear();

    std::vector<std::pair<int, int>> arcs;

    // список исходящих ребер для каждой вершины
    for (int u = 0; u < n; u++) {
        for (int i = 0; i < outDegrees[u]; i++) {
            arcs.push_back({u, -1});
        }
    }

    std::vector<int> inRemaining = inDegrees;

    std::random_device rd;
    std::mt19937 g(rd());

    std::set<std::pair<int, int>> usedArcs;

    for (auto& arc : arcs) {
        int u = arc.first;

        //только вершины v > u (верхняя треугольная матрица)
        // и с ненулевой остаточной входящей степенью чтоб ребро еще не было использовано
        std::vector<int> candidates;
        for (int v = u + 1; v < n; v++) {
            if (inRemaining[v] > 0 && usedArcs.find({u, v}) == usedArcs.end()) {
                candidates.push_back(v);
            }
        }

        // если таких нет ищем среди всех (но все равно исключаем петли)
        if (candidates.empty()) {
            for (int v = u + 1; v < n; v++) {
                if (v != u && usedArcs.find({u, v}) == usedArcs.end()) {
                    candidates.push_back(v);
                }
            }
        }

        // если да так стрем берем любую v > u (даже если inRemaining[v] == 0)
        if (candidates.empty()) {
            for (int v = u + 1; v < n; v++) {
                if (v != u) {
                    candidates.push_back(v);
                }
            }
        }

        if (!candidates.empty()) {
            std::uniform_int_distribution<int> dist(0, candidates.size() - 1);
            int v = candidates[dist(g)];
            arc.second = v;
            usedArcs.insert({u, v});  // коллекция использованных ребрышек
            if (inRemaining[v] > 0) {
                inRemaining[v]--;
            }
        }
    }

    //строим список смежности
    for (const auto& arc : arcs) {
        if (arc.second != -1 && arc.first < arc.second) {
            bool alreadyExists = false;
            for (int existing : adj[arc.first]) {
                if (existing == arc.second) {
                    alreadyExists = true;
                    break;
                }
            }
            if (!alreadyExists) {
                adj[arc.first].push_back(arc.second);
            }
        }
    }

    for (int i = 0; i < n; i++) {
        outDegrees[i] = adj[i].size();
    }

    std::vector<int> actualInDegrees(n, 0);
    for (int i = 0; i < n; i++) {
        for (int v : adj[i]) {
            actualInDegrees[v]++;
        }
    }
    inDegrees = actualInDegrees;

    return true;
}

bool Graph::isConnected() const //проверка связности
{
    if (n == 0) return true;

    std::vector<bool> visited(n, false);
    std::queue<int> q;

    q.push(0);
    visited[0] = true;
    int count = 1;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        for (int v : adj[u])
        {
            if (!visited[v])
            {
                visited[v] = true;
                count++;
                q.push(v);
            }
        }
    }

    return count == n;
}

bool Graph::isWeaklyConnected() const //проверка ориентированного на слабую связность
{
    if (type == UNDIRECTED) return isConnected();

    if (n == 0) return true;

    std::vector<bool> visited(n, false);
    std::queue<int> q;

    q.push(0);
    visited[0] = true;
    int count = 1;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        for (int v : adj[u])
        {
            if (!visited[v])
            {
                visited[v] = true;
                count++;
                q.push(v);
            }
        }
    }

    return count == n;
}

void Graph::print() const
{
    std::cout << "Graph (" << (type == UNDIRECTED ? "undirected" : "directed") << "):\n";
    std::cout << "Adjacency list:\n";
    for (int i = 0; i < n; i++)
    {
        std::cout << std::setw(2) << i << ": ";
        for (int v : adj[i])
        {
            std::cout << v << " ";
        }
        std::cout << "\n";
    }

    if (type == DIRECTED) {
        std::cout << "\nDegrees:\n";
        for (int i = 0; i < n; i++) {
            std::cout << "  Vertex " << i << ": out=" << outDegrees[i]
                      << ", in=" << inDegrees[i] << "\n";
        }
    }
}

//--------------------Lab 2----------------------------
void Graph::DFS(int start, std::vector<bool>& visited, std::vector<int>& traversal) const
{
    dfsIterations++;
    visited[start] = true;
    traversal.push_back(start);

    for (int neighbor : adj[start]) {
        dfsIterations++;
        if (!visited[neighbor]) {
            DFS(neighbor, visited, traversal);
        }
    }
}

std::vector<int> Graph::performDFS(int start) const
{
    dfsIterations = 0;
    std::vector<bool> visited(n, false);
    std::vector<int> traversal;

    DFS(start, visited, traversal);

    for (int i = 0; i < n; i++) {
        dfsIterations++;
        if (!visited[i]) {
            DFS(i, visited, traversal);
        }
    }

    return traversal;
}

std::vector<int> Graph::performDFSDirected(int start) const
{
    dfsIterations = 0;
    std::vector<bool> visited(n, false);
    std::vector<int> traversal;

    DFS(start, visited, traversal);

    return traversal;
}

//-----------------------------Lab 4--------------------------
std::vector<std::vector<int>> Graph::getKirchhoffMatrix() const{
    std::vector<std::vector<int>> matrix(n, std::vector<int>(n,0));
    for (int i = 0; i < n; i++){
        for (int neighbor : adj[i]){
            matrix[i][neighbor] = -1;
        }
        matrix[i][i] = adj[i].size();
    }
    return matrix;
}

//-----------------------------Lab 5--------------------------------

int Graph::getVertexDegree(int v) const
{
    return adj[v].size();
}

bool Graph::allDegreesEven() const
{
    for (int i = 0; i < n; i++) {
        if (adj[i].size() % 2 != 0) {
            return false;
        }
    }
    return true;
}

std::vector<int> Graph::getOddDegreeVertices() const
{
    std::vector<int> odd;
    for (int i = 0; i < n; i++) {
        if (adj[i].size() % 2 != 0) {
            odd.push_back(i);
        }
    }
    return odd;
}

bool Graph::addEdge(int u, int v)
{
    if (u < 0 || u >= n || v < 0 || v >= n || u == v) return false;

    // проверяем нет ли такого ребра
    for (int neighbor : adj[u]) {
        if (neighbor == v) return false;
    }

    adj[u].push_back(v);
    adj[v].push_back(u);

    if (type == UNDIRECTED) {
        degrees[u] = adj[u].size();
        degrees[v] = adj[v].size();
    }

    return true;
}

std::string Graph::makeEulerian()
{
    std::stringstream log;

    log << "╔══════════════════════════════════════════════════════════════════╗\n";
    log << "║              MODIFYING GRAPH TO EULERIAN                         ║\n";
    log << "╚══════════════════════════════════════════════════════════════════╝\n\n";

    if (type == DIRECTED) {
        log << "ERROR: Directed graphs not supported.\n";
        return log.str();
    }

    std::vector<int> originalDegrees(n, 0);
    for (int i = 0; i < n; i++) {
        originalDegrees[i] = adj[i].size();
    }

    log << "Original degrees:\n";
    for (int i = 0; i < n; i++) {
        log << "  Vertex " << i << ": " << originalDegrees[i];
        if (originalDegrees[i] % 2 != 0) log << " ← ODD";
        log << "\n";
    }
    log << "\n";

    int edgesAdded = 0;
    int edgesRemoved = 0;

    // продолжаем пока есть нечётные вершины
    int maxIterations = n * 2;
    int iter = 0;

    while (iter < maxIterations) {
        iter++;

        auto odd = getOddDegreeVertices();

        if (odd.empty()) {
            log << "✓ All degrees even! Done in " << iter << " iteration(s).\n";
            break;
        }

        if (odd.size() % 2 != 0) {
            log << "✗ ERROR: " << odd.size()
                << " odd-degree vertices (odd number) — impossible by Handshaking Lemma!\n";
            break;
        }

        log << "Iteration " << iter << ": " << odd.size() << " odd vertices: [";
        for (size_t i = 0; i < odd.size(); i++) {
            log << odd[i];
            if (i < odd.size() - 1) log << ", ";
        }
        log << "]\n";

        bool progress = false;

        // перебираем пары
        for (size_t i = 0; i < odd.size() && !progress; i++) {
            int u = odd[i];
            if (adj[u].size() % 2 == 0) continue; // уже чётная

            for (size_t j = i + 1; j < odd.size() && !progress; j++) {
                int v = odd[j];
                if (adj[v].size() % 2 == 0) continue; // уже чётная

                if (!hasEdge(u, v)) {
                    // случай 1: ребра нет -> добавляем
                    addEdge(u, v);
                    edgesAdded++;
                    progress = true;
                    log << "  + Added edge (" << u << ", " << v << ")\n";
                }
                else if (adj[u].size() >= 2 && adj[v].size() >= 2) {
                    // случай 2: ребро есть, но можно безопасно удалить
                    removeEdge(u, v);
                    edgesRemoved++;
                    progress = true;
                    log << "  - Removed edge (" << u << ", " << v << ")\n";
                }
                else {
                    // случай 3: ребро есть, но удалять нельзя (deg=1)
                    int weak = (adj[u].size() <= adj[v].size()) ? u : v;
                    int strong = (weak == u) ? v : u;

                    log << "  Edge (" << u << ", " << v
                        << ") exists, vertex " << weak
                        << " has deg=" << adj[weak].size() << " → restructuring\n";

                    // ищем две чётные соединенные вершины
                    for (int e1 = 0; e1 < n && !progress; e1++) {
                        if (e1 == u || e1 == v) continue;
                        if (adj[e1].size() < 2) continue;
                        if (adj[e1].size() % 2 != 0) continue;

                        for (int e2 = e1 + 1; e2 < n && !progress; e2++) {
                            if (e2 == u || e2 == v) continue;
                            if (adj[e2].size() < 2) continue;
                            if (adj[e2].size() % 2 != 0) continue;

                            if (!hasEdge(e1, e2)) continue;

                            // логика:
                            // удаление (u,v) — обе нечётные теряют по 1
                            // удаление (e1,e2) — обе чётные теряют по 1
                            // соединение weak с e1 и e2 — weak получает +2, e1 e2 получают +1

                            removeEdge(u, v);
                            removeEdge(e1, e2);
                            edgesRemoved += 2;

                            if (!hasEdge(weak, e1)) {
                                addEdge(weak, e1);
                                edgesAdded++;
                            }
                            if (!hasEdge(weak, e2)) {
                                addEdge(weak, e2);
                                edgesAdded++;
                            }

                            progress = true;
                            log << "    - Removed (" << u << ", " << v << ")\n";
                            log << "    - Removed (" << e1 << ", " << e2 << ") [even pair]\n";
                            log << "    + Connected " << weak << " to " << e1 << " and " << e2 << "\n";
                            log << "    Effect: " << weak << ": -1+2=+1, " << strong
                                << ": -1\n";
                        }
                    }

                    if (!progress) {
                        log << "    ✗ No suitable even pair found\n";
                    }
                }
            }
        }

        if (!progress) {
            log << "  No progress possible in this iteration.\n";

            // для всех оставшихся нечётных — просто добавляем связи
            auto finalOdd = getOddDegreeVertices();
            if (finalOdd.size() >= 2) {
                log << "  Force-connecting remaining odd vertices:\n";
                for (size_t i = 0; i < finalOdd.size(); i += 2) {
                    if (i + 1 >= finalOdd.size()) break;
                    int u = finalOdd[i];
                    int v = finalOdd[i + 1];
                    if (!hasEdge(u, v)) {
                        addEdge(u, v);
                        edgesAdded++;
                        log << "    + Added (" << u << ", " << v << ")\n";
                        progress = true;
                    }
                }
            }

            if (!progress) break;
        }
    }

    log << "\nFinal result:\n";
    log << "  Edges added: " << edgesAdded << "\n";
    log << "  Edges removed: " << edgesRemoved << "\n\n";

    for (int i = 0; i < n; i++) {
        log << "  Vertex " << i << ": deg=" << adj[i].size();
        if (adj[i].size() % 2 != 0) log << " ← ODD!";
        if (adj[i].size() == 0 && originalDegrees[i] > 0) log << " ← ISOLATED!";
        log << "\n";
    }

    if (allDegreesEven()) {
        log << "\n✓ SUCCESS: Graph is Eulerian!\n";
    } else {
        log << "\n✗ FAILED: Could not make all degrees even.\n";
    }

    return log.str();
}

bool Graph::removeEdge(int u, int v)
{
    if (u < 0 || u >= n || v < 0 || v >= n) return false;

    // удаление v из списка смежности u
    for (auto it = adj[u].begin(); it != adj[u].end(); ++it) {
        if (*it == v) {
            adj[u].erase(it);
            break;
        }
    }

    // удаление u из списка смежности v
    for (auto it = adj[v].begin(); it != adj[v].end(); ++it) {
        if (*it == u) {
            adj[v].erase(it);
            break;
        }
    }

    if (type == UNDIRECTED) {
        degrees[u] = adj[u].size();
        degrees[v] = adj[v].size();
    }

    return true;
}

bool Graph::hasEdge(int u, int v) const
{
    for (int neighbor : adj[u]) {
        if (neighbor == v) return true;
    }
    return false;
}

