/**
 * @file SmallGraph.cpp
 * @brief Creates a small, meaningful graph for pathfinding experiments.
 * 
 * This graph represents a simplified map of the NCSU campus with key buildings
 * and walkways between them.
 *
 * Author: Miles Hollifield
 * Date: 3/20/2025
 */

 #include "headers/Graph.h"
 #include <SFML/Graphics.hpp>
 #include <string>
 #include <vector>
 #include <iostream>
 
 /**
  * @brief Creates a graph representing a simplified map of NCSU campus.
  * @return Graph object representing the campus.
  */
 Graph createCampusGraph() {
     // Define our locations (vertices)
     const int NUM_LOCATIONS = 20;
     
     // Create the graph with the specified number of vertices
     Graph campusGraph(NUM_LOCATIONS);
     
     // Define positions for each vertex (for visualization and heuristics)
     std::vector<sf::Vector2f> positions = {
         {100, 100},   // 0: Talley Student Union
         {200, 100},   // 1: D.H. Hill Library
         {300, 150},   // 2: SAS Hall
         {350, 100},   // 3: Cox Hall
         {400, 150},   // 4: Engineering Building I
         {450, 200},   // 5: Engineering Building II
         {500, 250},   // 6: Engineering Building III
         {250, 200},   // 7: Daniels Hall
         {200, 250},   // 8: Riddick Hall
         {150, 250},   // 9: Mann Hall
         {150, 300},   // 10: Broughton Hall
         {250, 300},   // 11: Burlington Labs
         {350, 300},   // 12: Textiles Complex
         {450, 300},   // 13: Centennial Campus Center
         {100, 350},   // 14: Reynolds Coliseum
         {200, 350},   // 15: Carmichael Gym
         {300, 350},   // 16: Talley Student Center
         {400, 350},   // 17: Hunt Library
         {150, 50},    // 18: Bell Tower
         {300, 50}     // 19: Court of Carolinas
     };
     
     // Store positions in the graph
     campusGraph.setVertexPositions(positions);
     
     // Define edges (walkways between buildings)
     // Weight represents approximate walking time in minutes
     
     // Talley Student Union connections
     campusGraph.addEdge(0, 1, 2.0f);  // To D.H. Hill Library
     campusGraph.addEdge(0, 18, 3.0f); // To Bell Tower
     campusGraph.addEdge(0, 16, 5.0f); // To Talley Student Center
     campusGraph.addEdge(0, 9, 4.0f);  // To Mann Hall
     
     // D.H. Hill Library connections
     campusGraph.addEdge(1, 0, 2.0f);  // To Talley Student Union
     campusGraph.addEdge(1, 2, 3.0f);  // To SAS Hall
     campusGraph.addEdge(1, 19, 2.0f); // To Court of Carolinas
     campusGraph.addEdge(1, 7, 2.5f);  // To Daniels Hall
     
     // SAS Hall connections
     campusGraph.addEdge(2, 1, 3.0f);  // To D.H. Hill Library
     campusGraph.addEdge(2, 3, 2.0f);  // To Cox Hall
     campusGraph.addEdge(2, 7, 1.5f);  // To Daniels Hall
     
     // Cox Hall connections
     campusGraph.addEdge(3, 2, 2.0f);  // To SAS Hall
     campusGraph.addEdge(3, 4, 1.5f);  // To Engineering Building I
     campusGraph.addEdge(3, 19, 2.0f); // To Court of Carolinas
     
     // Engineering Building I connections
     campusGraph.addEdge(4, 3, 1.5f);  // To Cox Hall
     campusGraph.addEdge(4, 5, 1.0f);  // To Engineering Building II
     campusGraph.addEdge(4, 7, 3.0f);  // To Daniels Hall
     
     // Engineering Building II connections
     campusGraph.addEdge(5, 4, 1.0f);  // To Engineering Building I
     campusGraph.addEdge(5, 6, 1.0f);  // To Engineering Building III
     campusGraph.addEdge(5, 13, 3.0f); // To Centennial Campus Center
     
     // Engineering Building III connections
     campusGraph.addEdge(6, 5, 1.0f);  // To Engineering Building II
     campusGraph.addEdge(6, 12, 2.0f); // To Textiles Complex
     campusGraph.addEdge(6, 13, 2.5f); // To Centennial Campus Center
     campusGraph.addEdge(6, 17, 3.0f); // To Hunt Library
     
     // Daniels Hall connections
     campusGraph.addEdge(7, 1, 2.5f);  // To D.H. Hill Library
     campusGraph.addEdge(7, 2, 1.5f);  // To SAS Hall
     campusGraph.addEdge(7, 4, 3.0f);  // To Engineering Building I
     campusGraph.addEdge(7, 8, 1.0f);  // To Riddick Hall
     
     // Riddick Hall connections
     campusGraph.addEdge(8, 7, 1.0f);  // To Daniels Hall
     campusGraph.addEdge(8, 9, 1.0f);  // To Mann Hall
     campusGraph.addEdge(8, 11, 1.5f); // To Burlington Labs
     
     // Mann Hall connections
     campusGraph.addEdge(9, 0, 4.0f);  // To Talley Student Union
     campusGraph.addEdge(9, 8, 1.0f);  // To Riddick Hall
     campusGraph.addEdge(9, 10, 1.0f); // To Broughton Hall
     
     // Broughton Hall connections
     campusGraph.addEdge(10, 9, 1.0f);  // To Mann Hall
     campusGraph.addEdge(10, 11, 2.0f); // To Burlington Labs
     campusGraph.addEdge(10, 14, 2.0f); // To Reynolds Coliseum
     
     // Burlington Labs connections
     campusGraph.addEdge(11, 8, 1.5f);  // To Riddick Hall
     campusGraph.addEdge(11, 10, 2.0f); // To Broughton Hall
     campusGraph.addEdge(11, 12, 2.0f); // To Textiles Complex
     campusGraph.addEdge(11, 15, 1.5f); // To Carmichael Gym
     
     // Textiles Complex connections
     campusGraph.addEdge(12, 6, 2.0f);  // To Engineering Building III
     campusGraph.addEdge(12, 11, 2.0f); // To Burlington Labs
     campusGraph.addEdge(12, 13, 2.0f); // To Centennial Campus Center
     campusGraph.addEdge(12, 16, 1.5f); // To Talley Student Center
     
     // Centennial Campus Center connections
     campusGraph.addEdge(13, 5, 3.0f);  // To Engineering Building II
     campusGraph.addEdge(13, 6, 2.5f);  // To Engineering Building III
     campusGraph.addEdge(13, 12, 2.0f); // To Textiles Complex
     campusGraph.addEdge(13, 17, 1.5f); // To Hunt Library
     
     // Reynolds Coliseum connections
     campusGraph.addEdge(14, 10, 2.0f); // To Broughton Hall
     campusGraph.addEdge(14, 15, 1.5f); // To Carmichael Gym
     
     // Carmichael Gym connections
     campusGraph.addEdge(15, 11, 1.5f); // To Burlington Labs
     campusGraph.addEdge(15, 14, 1.5f); // To Reynolds Coliseum
     campusGraph.addEdge(15, 16, 2.0f); // To Talley Student Center
     
     // Talley Student Center connections
     campusGraph.addEdge(16, 0, 5.0f);  // To Talley Student Union
     campusGraph.addEdge(16, 12, 1.5f); // To Textiles Complex
     campusGraph.addEdge(16, 15, 2.0f); // To Carmichael Gym
     campusGraph.addEdge(16, 17, 2.5f); // To Hunt Library
     
     // Hunt Library connections
     campusGraph.addEdge(17, 6, 3.0f);  // To Engineering Building III
     campusGraph.addEdge(17, 13, 1.5f); // To Centennial Campus Center
     campusGraph.addEdge(17, 16, 2.5f); // To Talley Student Center
     
     // Bell Tower connections
     campusGraph.addEdge(18, 0, 3.0f);  // To Talley Student Union
     campusGraph.addEdge(18, 19, 3.0f); // To Court of Carolinas
     
     // Court of Carolinas connections
     campusGraph.addEdge(19, 1, 2.0f);  // To D.H. Hill Library
     campusGraph.addEdge(19, 3, 2.0f);  // To Cox Hall
     campusGraph.addEdge(19, 18, 3.0f); // To Bell Tower
     
     // Save graph to file for later use
     campusGraph.saveToFile("campus_graph.dat");
     
     return campusGraph;
 }
 
 /**
  * @brief Visualize the campus graph using SFML.
  * @param graph The graph to visualize.
  * @param locationNames Vector of location names for each vertex.
  */
 void visualizeCampusGraph(const Graph& graph, const std::vector<std::string>& locationNames) {
     // Create window
     sf::RenderWindow window(sf::VideoMode(800, 600), "NCSU Campus Graph");
     
     // Create font for text display
     sf::Font font;
     if (!font.loadFromFile("arial.ttf")) {
         std::cerr << "Error loading font!" << std::endl;
         return;
     }
     
     // Create shapes for vertices and edges
     std::vector<sf::CircleShape> vertices;
     std::vector<sf::RectangleShape> edges;
     std::vector<sf::Text> labels;
     
     // Set up vertices
     for (int i = 0; i < graph.size(); i++) {
         sf::Vector2f pos = graph.getVertexPosition(i);
         
         // Create vertex circle
         sf::CircleShape vertex(10);
         vertex.setPosition(pos - sf::Vector2f(10, 10)); // Center circle on position
         vertex.setFillColor(sf::Color::Blue);
         vertices.push_back(vertex);
         
         // Create label text
         sf::Text label(locationNames[i], font, 12);
         label.setPosition(pos + sf::Vector2f(12, -6));
         label.setFillColor(sf::Color::Black);
         labels.push_back(label);
     }
     
     // Set up edges
     for (int i = 0; i < graph.size(); i++) {
         sf::Vector2f from = graph.getVertexPosition(i);
         
         for (const auto& edge : graph.getNeighbors(i)) {
             int to_idx = edge.first;
             float weight = edge.second;
             sf::Vector2f to = graph.getVertexPosition(to_idx);
             
             // Calculate edge direction
             sf::Vector2f direction = to - from;
             float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
             
             if (length > 0) {
                 direction /= length;
                 
                 // Create a rectangle to represent the edge
                 sf::RectangleShape edgeShape(sf::Vector2f(length - 20, 2)); // Subtract diameter of vertices
                 edgeShape.setPosition(from + direction * 10.0f);
                 edgeShape.setRotation(std::atan2(direction.y, direction.x) * 180 / 3.14159265f);
                 edgeShape.setFillColor(sf::Color(100, 100, 100, 150));
                 edges.push_back(edgeShape);
                 
                 // Add weight label on edge
                 sf::Text weightLabel(std::to_string(static_cast<int>(weight)), font, 12);
                 sf::Vector2f midpoint = from + (to - from) * 0.5f;
                 weightLabel.setPosition(midpoint);
                 weightLabel.setFillColor(sf::Color::Red);
                 labels.push_back(weightLabel);
             }
         }
     }
     
     // Main rendering loop
     while (window.isOpen()) {
         sf::Event event;
         while (window.pollEvent(event)) {
             if (event.type == sf::Event::Closed) {
                 window.close();
             }
         }
         
         window.clear(sf::Color::White);
         
         // Draw edges first
         for (const auto& edge : edges) {
             window.draw(edge);
         }
         
         // Draw vertices
         for (const auto& vertex : vertices) {
             window.draw(vertex);
         }
         
         // Draw labels
         for (const auto& label : labels) {
             window.draw(label);
         }
         
         window.display();
     }
 }
 
 /**
  * @brief Creates location names for the campus graph.
  * @return Vector of location names.
  */
 std::vector<std::string> createLocationNames() {
     return {
         "Talley Student Union",       // 0
         "D.H. Hill Library",          // 1
         "SAS Hall",                   // 2
         "Cox Hall",                   // 3
         "Engineering Building I",     // 4
         "Engineering Building II",    // 5
         "Engineering Building III",   // 6
         "Daniels Hall",               // 7
         "Riddick Hall",               // 8
         "Mann Hall",                  // 9
         "Broughton Hall",             // 10
         "Burlington Labs",            // 11
         "Textiles Complex",           // 12
         "Centennial Campus Center",   // 13
         "Reynolds Coliseum",          // 14
         "Carmichael Gym",             // 15
         "Talley Student Center",      // 16
         "Hunt Library",               // 17
         "Bell Tower",                 // 18
         "Court of Carolinas"          // 19
     };
 }
 
 /**
  * @brief Main function for testing the campus graph.
  * @return 0 on success.
  */
 int main() {
     // Create the graph
     Graph campusGraph = createCampusGraph();
     
     // Create location names
     std::vector<std::string> locationNames = createLocationNames();
     
     // Visualize the graph
     visualizeCampusGraph(campusGraph, locationNames);
     
     return 0;
 }