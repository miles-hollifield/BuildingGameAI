/**
 * @file Heuristics.h
 * @brief Defines heuristic functions for A* pathfinding.
 *
 * Author: Miles Hollifield
 * Date: 3/20/2025
 */

 #ifndef HEURISTICS_H
 #define HEURISTICS_H
 
 #include "Graph.h"
 #include <cmath>
 #include <vector>
 #include <SFML/System/Vector2.hpp>
 #include <algorithm>
 
 namespace Heuristics {
     
     /**
      * @brief Euclidean distance heuristic (admissible and consistent).
      * @param current Current vertex.
      * @param goal Goal vertex.
      * @param graph The graph with vertex positions.
      * @return Estimated cost from current to goal.
      */
     inline float euclidean(int current, int goal, const Graph& graph) {
         sf::Vector2f currentPos = graph.getVertexPosition(current);
         sf::Vector2f goalPos = graph.getVertexPosition(goal);
         
         float dx = goalPos.x - currentPos.x;
         float dy = goalPos.y - currentPos.y;
         
         return std::sqrt(dx * dx + dy * dy);
     }
     
     /**
      * @brief Manhattan distance heuristic (admissible and consistent for grid-like graphs).
      * @param current Current vertex.
      * @param goal Goal vertex.
      * @param graph The graph with vertex positions.
      * @return Estimated cost from current to goal.
      */
     inline float manhattan(int current, int goal, const Graph& graph) {
         sf::Vector2f currentPos = graph.getVertexPosition(current);
         sf::Vector2f goalPos = graph.getVertexPosition(goal);
         
         return std::abs(goalPos.x - currentPos.x) + std::abs(goalPos.y - currentPos.y);
     }
     
     /**
      * @brief Cluster heuristic that groups vertices into clusters.
      * @param current Current vertex.
      * @param goal Goal vertex.
      * @param graph The graph.
      * @param clusters Map of vertex to cluster id.
      * @param clusterDistances Map of cluster pairs to distances.
      * @return Estimated cost from current to goal.
      */
     inline float cluster(int current, int goal, const Graph& graph, 
                         const std::unordered_map<int, int>& clusters,
                         const std::unordered_map<std::pair<int, int>, float, 
                         PairHash>& clusterDistances) {
         
         // If vertices are in the same cluster, use Euclidean distance
         if (clusters.at(current) == clusters.at(goal)) {
             return euclidean(current, goal, graph);
         }
         
         // Otherwise, use pre-computed cluster distances
         auto key = std::make_pair(clusters.at(current), clusters.at(goal));
         auto reversedKey = std::make_pair(clusters.at(goal), clusters.at(current));
         
         if (clusterDistances.find(key) != clusterDistances.end()) {
             return clusterDistances.at(key);
         } else if (clusterDistances.find(reversedKey) != clusterDistances.end()) {
             return clusterDistances.at(reversedKey);
         }
         
         // Fallback to Euclidean if cluster distance not found
         return euclidean(current, goal, graph);
     }
     
     /**
      * @brief Intentionally inadmissible heuristic that overestimates costs.
      * @param current Current vertex.
      * @param goal Goal vertex.
      * @param graph The graph with vertex positions.
      * @return Overestimated cost from current to goal.
      */
     inline float inadmissible(int current, int goal, const Graph& graph) {
         // Calculate Euclidean distance
         float euclideanDist = euclidean(current, goal, graph);
         
         // Apply overestimation factors
         float overestimationFactor = 1.5f;
         
         // More aggressive overestimation for distant nodes
         if (euclideanDist > 100.0f) {
             overestimationFactor = 2.0f;
         } else if (euclideanDist > 50.0f) {
             overestimationFactor = 1.75f;
         }
         
         // Add small random variation for non-determinism
         float randomVariation = static_cast<float>(rand() % 10) / 10.0f;
         
         return euclideanDist * overestimationFactor + randomVariation;
     }
     
     /**
      * @brief Alternative inadmissible heuristic that systematically overestimates in certain directions.
      * @param current Current vertex.
      * @param goal Goal vertex.
      * @param graph The graph with vertex positions.
      * @return Direction-biased overestimated cost.
      */
     inline float directionalBias(int current, int goal, const Graph& graph) {
         sf::Vector2f currentPos = graph.getVertexPosition(current);
         sf::Vector2f goalPos = graph.getVertexPosition(goal);
         
         float dx = goalPos.x - currentPos.x;
         float dy = goalPos.y - currentPos.y;
         float euclideanDist = std::sqrt(dx * dx + dy * dy);
         
         // Apply directional bias (e.g., penalize vertical movement more)
         float verticalBias = 2.0f;
         float horizontalBias = 1.2f;
         
         float biasedDist = std::abs(dx) * horizontalBias + std::abs(dy) * verticalBias;
         
         // Return the maximum of biased and euclidean to ensure inadmissibility
         return std::max(biasedDist, euclideanDist * 1.1f);
     }
     
     // Helper struct for unordered_map of pairs
     struct PairHash {
         template <class T1, class T2>
         std::size_t operator() (const std::pair<T1, T2>& pair) const {
             return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
         }
     };
 };
 
 #endif // HEURISTICS_H