/**
 * @file hw3.cpp
 * @brief Main application for pathfinding and path following demonstration.
 *
 * CSC 584/484 Spring 2025 Homework 3
 * Author: Miles Hollifield
 * Date: 3/20/2025
 */

 #include <SFML/Graphics.hpp>
 #include <iostream>
 #include <string>
 #include <vector>
 #include <chrono>
 
 // Include headers
 #include "headers/Graph.h"
 #include "headers/Dijkstra.h"
 #include "headers/AStar.h"
 #include "headers/Heuristics.h"
 #include "headers/Environment.h"
 #include "headers/PathFollower.h"
 
 // Function to create an indoor environment
 Environment createIndoorEnvironment(int width, int height) {
     Environment env(width, height);
     
     // Add rooms
     env.addRoom(sf::FloatRect(50, 50, 350, 250));      // Room 1
     env.addRoom(sf::FloatRect(450, 50, 300, 250));     // Room 2
     env.addRoom(sf::FloatRect(200, 350, 400, 200));    // Room 3
     
     // Add obstacles
     env.addObstacle(sf::FloatRect(150, 150, 50, 50));  // Obstacle in Room 1
     env.addObstacle(sf::FloatRect(500, 150, 100, 50)); // Obstacle in Room 2
     env.addObstacle(sf::FloatRect(300, 400, 50, 100)); // Obstacle in Room 3
     
     return env;
 }
 
 int main() {
     // Create window
     int windowWidth = 800;
     int windowHeight = 600;
     sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), 
                            "CSC 584/484 - Pathfinding and Path Following");
     
     // Load texture
     sf::Texture agentTexture;
     if (!agentTexture.loadFromFile("boid.png")) {
         std::cerr << "Failed to load texture!" << std::endl;
         return -1;
     }
     
     // Create environment
     Environment environment = createIndoorEnvironment(windowWidth, windowHeight);
     
     // Create graph representation of the environment (20px grid size)
     int gridSize = 20;
     Graph environmentGraph = environment.createGraph(gridSize);
     
     // Create pathfinding algorithms
     Dijkstra dijkstra;
     
     // A* with Euclidean distance heuristic (admissible and consistent)
     AStar astarEuclidean([](int current, int goal, const Graph& g) {
         return Heuristics::euclidean(current, goal, g);
     });
     
     // A* with Manhattan distance heuristic (admissible and consistent in grid environments)
     AStar astarManhattan([](int current, int goal, const Graph& g) {
         return Heuristics::manhattan(current, goal, g);
     });
     
     // A* with inadmissible heuristic
     AStar astarInadmissible([](int current, int goal, const Graph& g) {
         return Heuristics::inadmissible(current, goal, g);
     });
     
     // Create path follower
     PathFollower agent(sf::Vector2f(100, 100), agentTexture);
     
     // Create font for text
     sf::Font font;
     if (!font.loadFromFile("arial.ttf")) {
         std::cerr << "Failed to load font!" << std::endl;
         // Continue without text
     }
     
     // Text for instructions
     sf::Text instructionText;
     instructionText.setFont(font);
     instructionText.setString("Left-click: Find path with A* (Euclidean)\n"
                               "Right-click: Find path with Dijkstra\n"
                               "Middle-click: Find path with A* (Inadmissible)");
     instructionText.setCharacterSize(14);
     instructionText.setFillColor(sf::Color::Black);
     instructionText.setPosition(10, 10);
     
     // Text for algorithm statistics
     sf::Text statsText;
     statsText.setFont(font);
     statsText.setCharacterSize(14);
     statsText.setFillColor(sf::Color::Black);
     statsText.setPosition(10, windowHeight - 80);
     
     // Algorithm stats
     int nodesExplored = 0;
     int maxFringe = 0;
     float pathCost = 0.0f;
     float computeTime = 0.0f;
     std::string algoName = "None";
     
     // Clock for timing
     sf::Clock clock;
     
     // Main loop
     while (window.isOpen()) {
         // Handle events
         sf::Event event;
         while (window.pollEvent(event)) {
             if (event.type == sf::Event::Closed) {
                 window.close();
             } else if (event.type == sf::Event::MouseButtonPressed) {
                 // Get mouse position
                 sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                 
                 // Get current agent position
                 sf::Vector2f agentPos = agent.getPosition();
                 
                 // Convert positions to graph vertices
                 int startVertex = environment.pointToVertex(agentPos);
                 int goalVertex = environment.pointToVertex(mousePos);
                 
                 // Skip if vertices are the same
                 if (startVertex == goalVertex) {
                     continue;
                 }
                 
                 // Measure the time to compute the path
                 auto startTime = std::chrono::high_resolution_clock::now();
                 
                 std::vector<int> path;
                 
                 // Choose algorithm based on mouse button
                 if (event.mouseButton.button == sf::Mouse::Left) {
                     // Use A* with Euclidean heuristic
                     path = astarEuclidean.findPath(environmentGraph, startVertex, goalVertex);
                     nodesExplored = astarEuclidean.getNodesExplored();
                     maxFringe = astarEuclidean.getMaxFringeSize();
                     pathCost = astarEuclidean.getPathCost();
                     algoName = "A* (Euclidean)";
                 } else if (event.mouseButton.button == sf::Mouse::Right) {
                     // Use Dijkstra's algorithm
                     path = dijkstra.findPath(environmentGraph, startVertex, goalVertex);
                     nodesExplored = dijkstra.getNodesExplored();
                     maxFringe = dijkstra.getMaxFringeSize();
                     pathCost = dijkstra.getPathCost();
                     algoName = "Dijkstra";
                 } else if (event.mouseButton.button == sf::Mouse::Middle) {
                     // Use A* with inadmissible heuristic
                     path = astarInadmissible.findPath(environmentGraph, startVertex, goalVertex);
                     nodesExplored = astarInadmissible.getNodesExplored();
                     maxFringe = astarInadmissible.getMaxFringeSize();
                     pathCost = astarInadmissible.getPathCost();
                     algoName = "A* (Inadmissible)";
                 }
                 
                 auto endTime = std::chrono::high_resolution_clock::now();
                 std::chrono::duration<double, std::milli> duration = endTime - startTime;
                 computeTime = duration.count();
                 
                 // Convert path to waypoints
                 std::vector<sf::Vector2f> waypoints;
                 for (int vertex : path) {
                     waypoints.push_back(environment.vertexToPoint(vertex));
                 }
                 
                 // Set the path for the agent to follow
                 agent.setPath(waypoints);
                 
                 // Update stats text
                 std::string statsString = "Algorithm: " + algoName + "\n" +
                                           "Nodes Explored: " + std::to_string(nodesExplored) + "\n" +
                                           "Max Fringe Size: " + std::to_string(maxFringe) + "\n" +
                                           "Path Cost: " + std::to_string(pathCost) + "\n" +
                                           "Compute Time: " + std::to_string(computeTime) + " ms";
                 statsText.setString(statsString);
             }
         }
         
         // Update
         float deltaTime = clock.restart().asSeconds();
         agent.update(deltaTime);
         
         // Render
         window.clear(sf::Color::White);
         
         // Draw environment
         environment.draw(window);
         
         // Draw agent and path
         agent.draw(window);
         
         // Draw text
         window.draw(instructionText);
         window.draw(statsText);
         
         window.display();
     }
     
     return 0;
 }