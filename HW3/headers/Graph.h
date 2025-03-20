/**
 * @file Graph.h
 * @brief Defines the Graph class for representing weighted directed graphs.
 *
 * Author: Miles Hollifield
 * Date: 3/20/2025
 */

 #ifndef GRAPH_H
 #define GRAPH_H
 
 #include <vector>
 #include <unordered_map>
 #include <string>
 #include <fstream>
 #include <iostream>
 #include <SFML/System.hpp>
 
 /**
  * @class Graph
  * @brief Represents a weighted directed graph.
  */
 class Graph {
 public:
     /**
      * @brief Constructor for creating a graph with n vertices.
      * @param n Number of vertices.
      */
     Graph(int n = 0) : adjacencyList(n) {}
 
     /**
      * @brief Add a directed edge with a weight.
      * @param from Source vertex.
      * @param to Destination vertex.
      * @param weight Edge weight (must be positive).
      * @return True if edge was added successfully, false otherwise.
      */
     bool addEdge(int from, int to, float weight) {
         if (from < 0 || from >= adjacencyList.size() || 
             to < 0 || to >= adjacencyList.size() || 
             weight <= 0) {
             return false;
         }
         
         // Add edge
         adjacencyList[from].push_back({to, weight});
         return true;
     }
 
     /**
      * @brief Get neighbors of a vertex and their edge weights.
      * @param vertex The vertex to get neighbors for.
      * @return Vector of pairs (neighbor, weight).
      */
     std::vector<std::pair<int, float>> getNeighbors(int vertex) const {
         if (vertex < 0 || vertex >= adjacencyList.size()) {
             return {};
         }
         return adjacencyList[vertex];
     }
 
     /**
      * @brief Get number of vertices in the graph.
      * @return Number of vertices.
      */
     int size() const {
         return adjacencyList.size();
     }
 
     /**
      * @brief Save graph to a file.
      * @param filename Name of the file to save to.
      * @return True if successful, false otherwise.
      */
     bool saveToFile(const std::string& filename) const {
         std::ofstream file(filename);
         if (!file.is_open()) {
             return false;
         }
 
         // Write number of vertices
         file << adjacencyList.size() << std::endl;
 
         // Write edges
         for (int i = 0; i < adjacencyList.size(); i++) {
             for (const auto& edge : adjacencyList[i]) {
                 file << i << " " << edge.first << " " << edge.second << std::endl;
             }
         }
 
         file.close();
         return true;
     }
 
     /**
      * @brief Load graph from a file.
      * @param filename Name of the file to load from.
      * @return True if successful, false otherwise.
      */
     bool loadFromFile(const std::string& filename) {
         std::ifstream file(filename);
         if (!file.is_open()) {
             return false;
         }
 
         // Read number of vertices
         int numVertices;
         file >> numVertices;
         adjacencyList.resize(numVertices);
         for (auto& list : adjacencyList) {
             list.clear();
         }
 
         // Read edges
         int from, to;
         float weight;
         while (file >> from >> to >> weight) {
             if (!addEdge(from, to, weight)) {
                 file.close();
                 return false;
             }
         }
 
         file.close();
         return true;
     }
 
     /**
      * @brief Set vertex positions for spatial algorithms.
      * @param positions Vector of positions for each vertex.
      */
     void setVertexPositions(const std::vector<sf::Vector2f>& positions) {
         vertexPositions = positions;
     }
 
     /**
      * @brief Get position of a vertex.
      * @param vertex Vertex index.
      * @return Position of the vertex.
      */
     sf::Vector2f getVertexPosition(int vertex) const {
         if (vertex < 0 || vertex >= vertexPositions.size()) {
             return {0, 0};
         }
         return vertexPositions[vertex];
     }
 
 private:
     std::vector<std::vector<std::pair<int, float>>> adjacencyList;
     std::vector<sf::Vector2f> vertexPositions;
 };
 
 #endif // GRAPH_H