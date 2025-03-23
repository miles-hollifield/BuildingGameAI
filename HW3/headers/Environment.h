/**
 * @file Environment.h
 * @brief Defines the Environment class for creating indoor environments.
 *
 * Author: Miles Hollifield
 * Date: 3/20/2025
 */

 #ifndef ENVIRONMENT_H
 #define ENVIRONMENT_H
 
 #include <SFML/Graphics.hpp>
 #include <vector>
 #include <cmath>
 #include "Graph.h"
 
 /**
  * @class Environment
  * @brief Represents an indoor environment with rooms and obstacles.
  */
 class Environment {
 public:
     /**
      * @brief Constructor for creating an environment with specified dimensions.
      * @param width Width of the environment.
      * @param height Height of the environment.
      */
     Environment(int width, int height) : width(width), height(height) {}
     
     /**
      * @brief Add a room to the environment.
      * @param room Rectangle representing the room.
      */
     void addRoom(const sf::FloatRect& room) {
         rooms.push_back(room);
     }
     
     /**
      * @brief Add an obstacle to the environment.
      * @param obstacle Rectangle representing the obstacle.
      */
     void addObstacle(const sf::FloatRect& obstacle) {
         obstacles.push_back(obstacle);
     }
     
     /**
      * @brief Create a graph representation of the environment.
      * @param gridSize Size of the grid cells.
      * @return Graph representing the environment.
      */
     Graph createGraph(int gridSize) {
         // Calculate number of vertices in grid
         int cols = width / gridSize;
         int rows = height / gridSize;
         int numVertices = cols * rows;
         
         // Create graph
         Graph graph(numVertices);
         
         // Create vertex positions
         std::vector<sf::Vector2f> positions;
         vertexPositions.clear();
         
         for (int r = 0; r < rows; r++) {
             for (int c = 0; c < cols; c++) {
                 int idx = r * cols + c;
                 sf::Vector2f pos((c + 0.5f) * gridSize, (r + 0.5f) * gridSize);
                 positions.push_back(pos);
                 vertexPositions.push_back(pos);
             }
         }
         
         // Store positions in graph
         graph.setVertexPositions(positions);
         
         // Define direction arrays for 8-connected grid
         const int dx[] = {-1, 0, 1, 0, -1, -1, 1, 1}; 
         const int dy[] = {0, -1, 0, 1, -1, 1, -1, 1};
         
         for (int r = 0; r < rows; r++) {
             for (int c = 0; c < cols; c++) {
                 int idx = r * cols + c;
                 sf::Vector2f pos = positions[idx];
                 
                 // Skip if current position is inside an obstacle
                 if (isObstacle(pos)) {
                     continue;
                 }
                 
                 // Connect to adjacent vertices
                 for (int dir = 0; dir < 8; dir++) {
                     int nr = r + dy[dir];
                     int nc = c + dx[dir];
                     
                     // Check bounds
                     if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
                         continue;
                     }
                     
                     int nidx = nr * cols + nc;
                     sf::Vector2f npos = positions[nidx];
                     
                     // Skip if neighbor is in obstacle
                     if (isObstacle(npos)) {
                         continue;
                     }
                     
                     // Check if there's a clear path between vertices (no obstacles in the way)
                     if (hasLineOfSight(pos, npos)) {
                         // Calculate Euclidean distance as edge weight
                         float dx = npos.x - pos.x;
                         float dy = npos.y - pos.y;
                         float dist = std::sqrt(dx * dx + dy * dy);
                         
                         // Add edge with weight
                         graph.addEdge(idx, nidx, dist);
                     }
                 }
             }
         }
         
         gridCols = cols; // Store for conversion functions
         return graph;
     }
     
     /**
      * @brief Check if there's a clear line of sight between two points.
      * @param from Start point.
      * @param to End point.
      * @return True if there's a clear line of sight.
      */
     bool hasLineOfSight(const sf::Vector2f& from, const sf::Vector2f& to) {
         // Use Bresenham's line algorithm to check for obstacles between points
         float dx = std::abs(to.x - from.x);
         float dy = std::abs(to.y - from.y);
         
         int x = static_cast<int>(from.x);
         int y = static_cast<int>(from.y);
         
         int n = 1 + static_cast<int>(dx + dy);
         int x_inc = (to.x > from.x) ? 1 : -1;
         int y_inc = (to.y > from.y) ? 1 : -1;
         
         float error = dx - dy;
         dx *= 2;
         dy *= 2;
         
         // Check if there's an obstacle along the line
         for (; n > 0; --n) {
             sf::Vector2f checkPoint(static_cast<float>(x), static_cast<float>(y));
             
             if (isObstacle(checkPoint)) {
                 return false; // Obstacle in the way
             }
             
             if (error > 0) {
                 x += x_inc;
                 error -= dy;
             } else {
                 y += y_inc;
                 error += dx;
             }
         }
         
         return true; // No obstacles in the way
     }
     
     /**
      * @brief Convert a point in the environment to a vertex in the graph.
      * @param point The point to convert.
      * @return Vertex index in the graph.
      */
     int pointToVertex(const sf::Vector2f& point) const {
         // Find the closest vertex to the given point
         int bestVertex = -1;
         float bestDistance = std::numeric_limits<float>::max();
         
         for (int i = 0; i < vertexPositions.size(); i++) {
             float dx = point.x - vertexPositions[i].x;
             float dy = point.y - vertexPositions[i].y;
             float distance = dx * dx + dy * dy; // No need to sqrt for comparison
             
             if (distance < bestDistance) {
                 bestDistance = distance;
                 bestVertex = i;
             }
         }
         
         return bestVertex;
     }
     
     /**
      * @brief Convert a vertex in the graph to a point in the environment.
      * @param vertex Vertex index in the graph.
      * @return Point in the environment.
      */
     sf::Vector2f vertexToPoint(int vertex) const {
         if (vertex >= 0 && vertex < vertexPositions.size()) {
             return vertexPositions[vertex];
         }
         return {0, 0}; // Invalid vertex
     }
     
     /**
      * @brief Check if a point is inside an obstacle.
      * @param point The point to check.
      * @return True if the point is inside an obstacle.
      */
     bool isObstacle(const sf::Vector2f& point) const {
         // Check if point is outside environment bounds
         if (point.x < 0 || point.x >= width || point.y < 0 || point.y >= height) {
             return true;
         }
         
         // Check if point is inside any obstacle
         for (const auto& obstacle : obstacles) {
             if (obstacle.contains(point)) {
                 return true;
             }
         }
         
         // Check if point is outside all rooms
         bool insideAnyRoom = false;
         for (const auto& room : rooms) {
             if (room.contains(point)) {
                 insideAnyRoom = true;
                 break;
             }
         }
         
         return !insideAnyRoom; // If not inside any room, it's considered an obstacle
     }
     
     /**
      * @brief Draw the environment to a window.
      * @param window The window to draw to.
      */
     void draw(sf::RenderWindow& window) const {
         // Draw rooms
         for (const auto& room : rooms) {
             sf::RectangleShape roomShape({room.width, room.height});
             roomShape.setPosition({room.left, room.top});
             roomShape.setFillColor(sf::Color(240, 240, 240));
             roomShape.setOutlineColor(sf::Color::Black);
             roomShape.setOutlineThickness(2);
             window.draw(roomShape);
         }
         
         // Draw obstacles
         for (const auto& obstacle : obstacles) {
             sf::RectangleShape obstacleShape({obstacle.width, obstacle.height});
             obstacleShape.setPosition({obstacle.left, obstacle.top});
             obstacleShape.setFillColor(sf::Color(100, 100, 100));
             window.draw(obstacleShape);
         }
     }
     
 private:
     int width, height;
     std::vector<sf::FloatRect> rooms;
     std::vector<sf::FloatRect> obstacles;
     std::vector<sf::Vector2f> vertexPositions;
     int gridCols; // Number of columns in the grid
 };
 
 #endif // ENVIRONMENT_H