#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>

class Graph
{
public:
    enum GraphType { UNDIRECTED, DIRECTED };

private:
    int n;                                    // количество вершин
    GraphType type;                           // тип графа
    std::vector<std::vector<int>> adj;        // список смежности
    std::vector<int> degrees;                 // исходные степени (для неориент.)
    std::vector<int> inDegrees;               // входящие степени (для ориент.)
    std::vector<int> outDegrees;              // исходящие степени (для ориент.)
    mutable int dfsIterations;
    // lab 5
    std::string tryMakeEulerianByAdding(std::vector<int>& oddVertices, const std::vector<int>& originalDegrees);
    std::string tryMakeEulerianByRemoving(std::vector<int>& oddVertices,const std::vector<int>& originalDegrees);
    bool canRemoveEdgeWithoutDisconnecting(int u, int v) const;
    bool removeEdge(int u, int v);
    int findCommonNeighbor(int u, int v) const;
    bool canSafelyRemoveEdge(int u, int v) const;
    bool hasEdge(int u, int v) const;
    std::vector<int> findPathThroughEvenVertices(int u, int v) const;
    bool restructureEdgesForOddPair(int u, int v, std::stringstream& log, int& addedEdges, int& removedEdges);

    bool isConnected() const;
    bool isWeaklyConnected() const;

public:
    Graph(int vertices, GraphType t = UNDIRECTED);

    bool buildGraphFromDesiredDegrees(const std::vector<int>& desiredDegrees);
    bool buildDirectedGraphFromDesiredDegrees(const std::vector<int>& desiredOutDegrees,
                                         const std::vector<int>& desiredInDegrees);

    int getVerticesCount() const { return n; }
    GraphType getType() const { return type; }

    const std::vector<int>& getNeighbors(int v) const { return adj[v]; }
    const std::vector<std::vector<int>>& getAdjacencyList() const { return adj; }

    int getOutDegree(int v) const { return outDegrees[v]; }
    int getInDegree(int v) const { return inDegrees[v]; }

    void print() const;

    //--------------------Lab 2----------------------------
    void DFS(int start, std::vector<bool>& visited, std::vector<int>& traversal) const;
    std::vector<int> performDFS(int start) const;
    std::vector<int> performDFSDirected(int start) const;
    int getLastDFSIterations() const { return dfsIterations; }


    //--------------------Lab 4------------------------------
    std::vector<std::vector<int>> getKirchhoffMatrix() const;

    //--------------------Lab 5----------------------------
    bool allDegreesEven() const;
    std::vector<int> getOddDegreeVertices() const;
    bool addEdge(int u, int v);
    std::string makeEulerian();
    int getVertexDegree(int v) const;
    int getTotalEdges() const;



};

#endif
