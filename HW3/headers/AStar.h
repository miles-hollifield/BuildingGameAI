/**
 * @file AStar.h
 * @brief Implements the A* algorithm for finding shortest paths.
 *
 * Author: Miles Hollifield
 * Date: 3/20/2025
 */

 #ifndef ASTAR_H
 #define ASTAR_H
 
 #include "Pathfinder.h"
 #include <queue>
 #include <unordered_map>
 #include <functional>
 #include <limits>
 
 /**
  * @class AStar
  * @brief Implementation of A* algorithm for finding shortest paths.
  */
 class AStar : public Pathfinder {
 public:
     /**
      * @brief Constructor taking a heuristic function.
      * @param heuristic Function that estimates cost from current to goal.
      */
     AStar(std::function<float(int, int, const Graph&)> heuristic) 
         : heuristicFunc(heuristic) {}
 
     /**
      * @brief Find the shortest path from start to goal using A* algorithm.
      * @param graph The graph to search.
      * @param start Start vertex.
      * @param goal Goal vertex.
      * @return Vector of vertices representing the shortest path.
      */
     std::vector<int> findPath(const Graph& graph, int start, int goal) override {
         // Reset performance metrics
         nodesExplored = 0;
         maxFringeSize = 0;
         pathCost = 0.0f;
         
         // Initialize data structures
         // Priority queue ordering by f-score (g-score + heuristic)
         auto compare = [](const std::pair<float, int>& a, const std::pair<float, int>& b) {
             return a.first > b.first; // Min-heap
         };
         std::priority_queue<std::pair<float, int>, 
                            std::vector<std::pair<float, int>>, 
                            decltype(compare)> fringe(compare);
         
         std::unordered_map<int, float> gScore; // Cost from start to vertex
         std::unordered_map<int, int> cameFrom; // For path reconstruction
         std::unordered_map<int, bool> visited; // To track visited nodes
         
         // Initialize start node
         gScore[start] = 0.0f;
         float fScore = heuristicFunc(start, goal, graph);
         fringe.push({fScore, start});
         
         // Track maximum fringe size
         maxFringeSize = 1;
         
         // Main search loop
         while (!fringe.empty()) {
             // Get the node with lowest f-score
             int current = fringe.top().second;
             fringe.pop();
             
             // Skip if already visited
             if (visited[current]) {
                 continue;
             }
             
             // Mark as explored and visited
             nodesExplored++;
             visited[current] = true;
             
             // Check if we reached the goal
             if (current == goal) {
                 pathCost = gScore[current];
                 return reconstructPath(cameFrom, start, goal);
             }
             
             // Explore neighbors
             for (const auto& edge : graph.getNeighbors(current)) {
                 int next = edge.first;
                 float weight = edge.second;
                 float tentativeGScore = gScore[current] + weight;
                 
                 // If we found a better path to this node
                 if (gScore.find(next) == gScore.end() || tentativeGScore < gScore[next]) {
                     // Update path
                     cameFrom[next] = current;
                     gScore[next] = tentativeGScore;
                     float estimatedCost = tentativeGScore + heuristicFunc(next, goal, graph);
                     fringe.push({estimatedCost, next});
                 }
             }
             
             // Update maximum fringe size
             maxFringeSize = std::max(maxFringeSize, static_cast<int>(fringe.size()));
         }
         
         // No path found
         return {};
     }
 
 private:
     std::function<float(int, int, const Graph&)> heuristicFunc;
 };
 
 #endif // ASTAR_H