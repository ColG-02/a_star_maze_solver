#include <iostream>
#include <list>
#include <vector>
#include <queue>
#include <map>


class Graph {
private:

    std::map<int, std::list<int> > adjList; // Adjacency list to store the graph
    int size = adjList.size();

public:

    


    // Function to add an edge between vertices u and v of
    // the graph
    void add_edge(int u, int v)
    {
        // Add edge from u to v
        adjList[u].push_back(v);
        // Add edge from v to u because the graph is
        // undirected
        adjList[v].push_back(u);
        size = adjList.size();
    }

    // Function to print the adjacency list representation
    // of the graph
    void print()
    {
        std::cerr << "Adjacency list for the Graph: " << std::endl;
        // Iterate over each vertex
        for (auto i : adjList) {
            // Print the vertex
            std::cerr << i.first << " -> ";
            // Iterate over the connected vertices
            for (auto j : i.second) {
                // Print the connected vertex
                std::cerr << j << " ";
            }
            std::cerr << std::endl;
        }
    }

    std::vector<int> bfs_path(int start, int goal) {
        std::map<int, bool> visited;
        std::map<int, int> predecessor; // Track path
        std::queue<int> q;

        q.push(start);
        visited[start] = true;
        predecessor[start] = -1; // Initialize predecessor for start node

        while (!q.empty()) {
            int node = q.front();
            q.pop();

            if (node == goal) {
                std::vector<int> path;
                for (int at = goal; at != -1; at = predecessor[at]) {
                    path.push_back(at);
                }
                //std::reverse(path.begin(), path.end()); // Reverse to get the correct order
                return path;
            }

            for (int neighbor : adjList[node]) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    predecessor[neighbor] = node;
                    q.push(neighbor);
                }
            }
        }
        return {}; // Return empty if no path found
    }
};