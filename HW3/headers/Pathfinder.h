/**
 * @file Pathfinder.h
 * @brief Defines the abstract Pathfinder class for pathfinding algorithms.
 *
 * Author: Miles Hollifield
 * Date: 3/20/2025
 */

 #ifndef PATHFINDER_H
 #define PATHFINDER_H
 
 #include "Graph.h"
 #include <vector>
 #include <unordered_map>
 
 /**
  * @class Pathfinder
  * @brief Abstract base class for pathfinding algorithms.
  */
 class Pathfinder {
 public:
     Pathfinder() : nodesExplored(0), maxFringeSize(0), pathCost(0.0f) {}
     virtual ~Pathfinder() = default;
 
     /**
      * @brief Find a path from start to goal.
      * @param graph The graph to search.
      * @param start Start vertex.
      * @param goal Goal vertex.
      * @return Vector of vertices representing the path.
      */
     virtual std::vector<int> findPath(const Graph& graph, int start, int goal) = 0;
 
     /**
      * @brief Get number of nodes explored during search.
      * @return Number of nodes explored.
      */
     int getNodesExplored() const {
         return nodesExplored;
     }
 
     /**
      * @brief Get maximum fringe size during search.
      * @return Maximum fringe size.
      */
     int getMaxFringeSize() const {
         return maxFringeSize;
     }
 
     /**
      * @brief Get cost of the found path.
      * @return Path cost.
      */
     float getPathCost() const {
         return pathCost;
     }
 
     /**
      * @brief Reconstruct path from came_from map.
      * @param cameFrom Map of each vertex to previous vertex in the path.
      * @param start Start vertex.
      * @param goal Goal vertex.
      * @return Vector of vertices representing the path.
      */
     std::vector<int> reconstructPath(const std::unordered_map<int, int>& cameFrom, 
                                     int start, int goal) const {
         std::vector<int> path;
         int current = goal;
         
         // Check if path exists
         if (cameFrom.find(goal) == cameFrom.end() && start != goal) {
             return path; // No path found
         }
         
         // Reconstruct path
         while (current != start) {
             path.push_back(current);
             auto it = cameFrom.find(current);
             if (it == cameFrom.end()) {
                 return {}; // Path reconstruction failed
             }
             current = it->second;
         }
         
         path.push_back(start);
         std::reverse(path.begin(), path.end());
         return path;
     }
 
 protected:
     int nodesExplored;
     int maxFringeSize;
     float pathCost;
 };
 
 #endif // PATHFINDER_H