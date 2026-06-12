#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "Graph.h"
#include <vector>
#include <string>

class PathFinder
{
private:
    const Graph& graph;
    int n;

    // Рекурсивный поиск всех простых путей (без повторения вершин)
    void findAllSimplePaths(int current, int target, std::vector<bool>& visited,
                            std::vector<int>& currentPath,
                            std::vector<std::vector<int>>& allPaths) const;

public:
    PathFinder(const Graph& g);

    // Возвращает количество всех простых путей (без циклов)
    int countAllSimplePaths(int start, int end) const;

    // Возвращает все простые пути (без циклов)
    std::vector<std::vector<int>> findAllSimplePaths(int start, int end) const;

    // Проверяет существование пути
    bool pathExists(int start, int end) const;

    // Форматирует пути для отображения
    std::string formatPaths(int start, int end) const;
};

#endif // PATHFINDER_H
