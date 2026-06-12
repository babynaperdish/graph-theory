#include "PathFinder.h"
#include <queue>
#include <algorithm>
#include <limits>
#include <set>

PathFinder::PathFinder(const Graph& g)
    : graph(g), n(g.getVerticesCount())
{
}

void PathFinder::findAllSimplePaths(int current, int target, std::vector<bool>& visited,
                                    std::vector<int>& currentPath,
                                    std::vector<std::vector<int>>& allPaths) const
{
    currentPath.push_back(current);

    if (current == target)
    {
        allPaths.push_back(currentPath);
        currentPath.pop_back();
        return;
    }

    visited[current] = true;

    for (int neighbor : graph.getNeighbors(current))
    {
        if (!visited[neighbor])
        {
            findAllSimplePaths(neighbor, target, visited, currentPath, allPaths);
        }
    }

    visited[current] = false;
    currentPath.pop_back();
}

int PathFinder::countAllSimplePaths(int start, int end) const
{
    if (start < 0 || start >= n || end < 0 || end >= n)
        return 0;

    if (start == end) return 1;

    std::vector<bool> visited(n, false);
    std::vector<int> currentPath;
    std::vector<std::vector<int>> allPaths;

    findAllSimplePaths(start, end, visited, currentPath, allPaths);

    return allPaths.size();
}

std::vector<std::vector<int>> PathFinder::findAllSimplePaths(int start, int end) const
{
    std::vector<std::vector<int>> allPaths;
    if (start < 0 || start >= n || end < 0 || end >= n)
        return allPaths;

    if (start == end)
    {
        allPaths.push_back({start});
        return allPaths;
    }

    std::vector<bool> visited(n, false);
    std::vector<int> currentPath;

    findAllSimplePaths(start, end, visited, currentPath, allPaths);

    if (graph.getType() == Graph::DIRECTED)
    {
        std::vector<std::vector<int>> validPaths;
        for (const auto& path : allPaths)
        {
            bool valid = true;
            for (size_t i = 0; i < path.size() - 1; i++)
            {
                bool edgeExists = false;
                for (int neighbor : graph.getNeighbors(path[i]))
                {
                    if (neighbor == path[i + 1])
                    {
                        edgeExists = true;
                        break;
                    }
                }
                if (!edgeExists)
                {
                    valid = false;
                    break;
                }
            }
            if (valid)
            {
                validPaths.push_back(path);
            }
        }
        return validPaths;
    }

    return allPaths;
}

bool PathFinder::pathExists(int start, int end) const
{
    if (start < 0 || start >= n || end < 0 || end >= n)
        return false;

    if (start == end) return true;

    std::vector<bool> visited(n, false);
    std::queue<int> q;

    q.push(start);
    visited[start] = true;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        for (int v : graph.getNeighbors(u))
        {
            if (v == end) return true;
            if (!visited[v])
            {
                visited[v] = true;
                q.push(v);
            }
        }
    }

    return false;
}

std::string PathFinder::formatPaths(int start, int end) const
{
    std::string result;

    if (!pathExists(start, end))
    {
        result = "No route exists from " + std::to_string(start) + " to " + std::to_string(end);
        return result;
    }

    auto paths = findAllSimplePaths(start, end);

    result = "Routes from " + std::to_string(start) + " to " + std::to_string(end) + ":\n";
    result += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    result += "Total: " + std::to_string(paths.size()) + " route(s)\n\n";

    if (paths.empty())
    {
        result += "No routes found!\n";
        return result;
    }

    for (size_t i = 0; i < paths.size(); i++)
    {
        result += std::to_string(i + 1) + ". ";
        for (size_t j = 0; j < paths[i].size(); j++)
        {
            result += std::to_string(paths[i][j]);
            if (j < paths[i].size() - 1)
                result += " → ";
        }
        result += "\n";
    }

    return result;
}
