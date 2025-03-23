/**
 * @file Dijkstra.h
 * @brief Implements Dijkstra's algorithm for finding shortest paths.
 *
 * Resources Used:
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - AI Tools: OpenAI's ChatGPT
 *
 * Author: Miles Hollifield
 * Date: 3/20/2025
 */

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "Pathfinder.h"
#include <queue>
#include <unordered_map>
#include <limits>

/** OpenAI's ChatGPT was used to suggest a template header file for Dijkstra's
 * implementation. The following prompt was used: "Create a template header file
 * for a class that implements the Dijkstra algorithm for pathfinding in C++ using SFML."
 */

/**
 * @class Dijkstra
 * @brief Implementation of Dijkstra's algorithm for finding shortest paths.
 */
class Dijkstra : public Pathfinder
{
public:
    /**
     * @brief Find the shortest path from start to goal using Dijkstra's algorithm.
     * @param graph The graph to search.
     * @param start Start vertex.
     * @param goal Goal vertex.
     * @return Vector of vertices representing the shortest path.
     */
    std::vector<int> findPath(const Graph &graph, int start, int goal) override
    {
        // Reset performance metrics
        nodesExplored = 0;
        maxFringeSize = 0;
        pathCost = 0.0f;

        // Initialize data structures
        std::priority_queue<std::pair<float, int>,
                            std::vector<std::pair<float, int>>,
                            std::greater<std::pair<float, int>>>
            fringe;
        std::unordered_map<int, float> costSoFar;
        std::unordered_map<int, int> cameFrom;
        std::unordered_map<int, bool> visited;

        // Add start node to fringe
        fringe.push({0.0f, start});
        costSoFar[start] = 0.0f;

        // Track maximum fringe size
        maxFringeSize = 1;

        // Main search loop
        while (!fringe.empty())
        {
            // Get the node with lowest cost
            int current = fringe.top().second;
            float currentCost = fringe.top().first;
            fringe.pop();

            // Mark as explored
            nodesExplored++;

            // Skip if already visited
            if (visited[current])
            {
                continue;
            }

            // Mark as visited
            visited[current] = true;

            // Check if we reached the goal
            if (current == goal)
            {
                pathCost = currentCost;
                return reconstructPath(cameFrom, start, goal);
            }

            // Explore neighbors
            for (const auto &edge : graph.getNeighbors(current))
            {
                int next = edge.first;
                float weight = edge.second;
                float newCost = costSoFar[current] + weight;

                // If we found a better path to this node
                if (costSoFar.find(next) == costSoFar.end() || newCost < costSoFar[next])
                {
                    costSoFar[next] = newCost;
                    cameFrom[next] = current;
                    fringe.push({newCost, next});
                }
            }

            // Update maximum fringe size
            maxFringeSize = std::max(maxFringeSize, static_cast<int>(fringe.size()));
        }

        // No path found
        return {};
    }
};

#endif // DIJKSTRA_H