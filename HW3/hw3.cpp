/**
 * @file hw3.cpp
 * @brief Main application for pathfinding and path following demonstration.
 *
 * CSC 584/484 Spring 2025 Homework 3
 * Author: Miles Hollifield
 * Date: 3/22/2025
 */

 #include <SFML/Graphics.hpp>
 #include <iostream>
 #include <string>
 #include <vector>
 #include <chrono>
 #include <iomanip>
 
 // Include headers
 #include "headers/Graph.h"
 #include "headers/Dijkstra.h"
 #include "headers/AStar.h"
 #include "headers/Heuristics.h"
 #include "headers/Environment.h"
 #include "headers/PathFollower.h"
 
 /**
  * @brief Creates an indoor environment with multiple rooms and obstacles.
  * @param width Width of the environment.
  * @param height Height of the environment.
  * @return Environment object with rooms and obstacles.
  */
 Environment createIndoorEnvironment(int width, int height) {
    Environment env(width, height);
    
    // Create a large background room that encompasses the entire space
    env.addRoom(sf::FloatRect(50, 50, 700, 500));
    
    // Add walls to define rooms with openings for doorways
    
    // Top horizontal divider with doorway gap
    env.addObstacle(sf::FloatRect(50, 200, 300, 10));  // Left section
    env.addObstacle(sf::FloatRect(400, 200, 350, 10)); // Right section
    
    // Vertical divider with two doorway gaps
    env.addObstacle(sf::FloatRect(400, 50, 10, 100));  // Top section
    env.addObstacle(sf::FloatRect(400, 250, 10, 150)); // Middle section
    env.addObstacle(sf::FloatRect(400, 450, 10, 100)); // Bottom section
    
    // Left vertical divider with doorway
    env.addObstacle(sf::FloatRect(200, 200, 10, 150)); // Upper section
    env.addObstacle(sf::FloatRect(200, 400, 10, 150)); // Lower section
    
    // Right vertical divider with doorway
    env.addObstacle(sf::FloatRect(600, 200, 10, 150)); // Upper section
    env.addObstacle(sf::FloatRect(600, 400, 10, 150)); // Lower section
    
    // Bottom horizontal divider with doorway gap
    env.addObstacle(sf::FloatRect(50, 400, 100, 10));  // Left section
    env.addObstacle(sf::FloatRect(250, 400, 100, 10)); // Middle-left section
    env.addObstacle(sf::FloatRect(450, 400, 100, 10)); // Middle-right section
    env.addObstacle(sf::FloatRect(650, 400, 100, 10)); // Right section
    
    // Add obstacles within rooms
    // Top-left room obstacles
    env.addObstacle(sf::FloatRect(100, 100, 40, 40));
    
    // Top-right room obstacles
    env.addObstacle(sf::FloatRect(500, 100, 30, 60));
    
    // Middle-left room obstacles
    env.addObstacle(sf::FloatRect(100, 300, 50, 30));
    
    // Middle-center room obstacles
    env.addObstacle(sf::FloatRect(300, 300, 30, 70));
    
    // Middle-right room obstacles
    env.addObstacle(sf::FloatRect(480, 300, 70, 50));
    
    // Bottom row room obstacles
    env.addObstacle(sf::FloatRect(150, 480, 40, 40));
    env.addObstacle(sf::FloatRect(350, 480, 30, 40));
    env.addObstacle(sf::FloatRect(650, 450, 50, 30));
    
    return env;
}
 
 int main() {
     // Create window
     int windowWidth = 800;
     int windowHeight = 600;
     sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), 
                            "CSC 584/484 - Pathfinding and Path Following");
     
     // Load texture for the agent
     sf::Texture agentTexture;
     if (!agentTexture.loadFromFile("boid.png")) {
         std::cerr << "Failed to load boid.png! Creating fallback texture." << std::endl;
         
         // Create a fallback texture with a simple triangle
         agentTexture.create(32, 32);
         sf::Image img;
         img.create(32, 32, sf::Color::Transparent);
         
         // Draw a simple triangle
         for (int y = 0; y < 32; y++) {
             for (int x = 0; x < 32; x++) {
                 if (x >= 16-y/2 && x <= 16+y/2 && y < 24) {
                     img.setPixel(x, y, sf::Color::Green);
                 }
             }
         }
         agentTexture.update(img);
     }
     
     // Create environment
     std::cout << "Creating indoor environment..." << std::endl;
     Environment environment = createIndoorEnvironment(windowWidth, windowHeight);
     
     // Create graph representation of the environment
     std::cout << "Creating graph representation..." << std::endl;
     int gridSize = 20; // 20px grid cells
     Graph environmentGraph = environment.createGraph(gridSize);
     std::cout << "Graph created with " << environmentGraph.size() << " vertices" << std::endl;
     
     // Create pathfinding algorithms
     Dijkstra dijkstra;
     
     // A* with Euclidean distance heuristic
     AStar astar([](int current, int goal, const Graph& g) {
         return Heuristics::euclidean(current, goal, g);
     });
     
     // Create path follower (agent)
     sf::Vector2f startPos(100, 100); // Starting position
     PathFollower agent(startPos, agentTexture);
     
     // Create font for text display
     sf::Font font;
     bool fontLoaded = false;
     
     // Try to load different font options
     if (font.loadFromFile("arial.ttf")) {
         fontLoaded = true;
     } else if (font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
         fontLoaded = true;
     } else if (font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
         fontLoaded = true;
     }
     
     // Text for instructions
     sf::Text instructionText;
     if (fontLoaded) {
         instructionText.setFont(font);
         instructionText.setString("Left-click: Find path with A*\n"
                                  "Right-click: Find path with Dijkstra\n"
                                  "R: Reset agent position");
         instructionText.setCharacterSize(14);
         instructionText.setFillColor(sf::Color::Black);
         instructionText.setPosition(10, 10);
     }
     
     // Text for path statistics
     sf::Text statsText;
     if (fontLoaded) {
         statsText.setFont(font);
         statsText.setCharacterSize(14);
         statsText.setFillColor(sf::Color::Black);
         statsText.setPosition(10, windowHeight - 80);
         statsText.setString("Click somewhere to navigate");
     }
     
     // Path statistics variables
     std::string algorithmName = "None";
     int nodesExplored = 0;
     int maxFringe = 0;
     float pathCost = 0.0f;
     float computeTime = 0.0f;
     
     // Visualization for graph vertices
     std::vector<sf::CircleShape> graphVertices;
     bool showGraph = false; // Option to display graph points
     
     if (showGraph) {
         for (int i = 0; i < environmentGraph.size(); i++) {
             sf::Vector2f pos = environmentGraph.getVertexPosition(i);
             if (!environment.isObstacle(pos)) {
                 sf::CircleShape vertex(2);
                 vertex.setPosition(pos - sf::Vector2f(2, 2));
                 vertex.setFillColor(sf::Color(200, 200, 200, 128));
                 graphVertices.push_back(vertex);
             }
         }
     }
     
     // Clock for timing
     sf::Clock clock;
     
     // Main loop
     while (window.isOpen()) {
         // Handle events
         sf::Event event;
         while (window.pollEvent(event)) {
             if (event.type == sf::Event::Closed) {
                 window.close();
             } else if (event.type == sf::Event::KeyPressed) {
                 if (event.key.code == sf::Keyboard::Escape) {
                     window.close();
                 } else if (event.key.code == sf::Keyboard::R) {
                     // Reset agent position
                     agent.setPosition(startPos);
                     if (fontLoaded) {
                         statsText.setString("Agent position reset");
                     }
                 } else if (event.key.code == sf::Keyboard::G) {
                     // Toggle graph visibility
                     showGraph = !showGraph;
                 }
             } else if (event.type == sf::Event::MouseButtonPressed) {
                 // Get mouse position
                 sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                 
                 // Skip if clicking in an obstacle
                 if (environment.isObstacle(mousePos)) {
                     if (fontLoaded) {
                         statsText.setString("Cannot navigate to this location (obstacle)");
                     }
                     continue;
                 }
                 
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
                     // Use A* algorithm
                     path = astar.findPath(environmentGraph, startVertex, goalVertex);
                     nodesExplored = astar.getNodesExplored();
                     maxFringe = astar.getMaxFringeSize();
                     pathCost = astar.getPathCost();
                     algorithmName = "A*";
                 } else if (event.mouseButton.button == sf::Mouse::Right) {
                     // Use Dijkstra's algorithm
                     path = dijkstra.findPath(environmentGraph, startVertex, goalVertex);
                     nodesExplored = dijkstra.getNodesExplored();
                     maxFringe = dijkstra.getMaxFringeSize();
                     pathCost = dijkstra.getPathCost();
                     algorithmName = "Dijkstra";
                 }
                 
                 auto endTime = std::chrono::high_resolution_clock::now();
                 std::chrono::duration<double, std::milli> duration = endTime - startTime;
                 computeTime = duration.count();
                 
                 // Check if a path was found
                 if (path.empty()) {
                     if (fontLoaded) {
                         statsText.setString("No path found!");
                     }
                     continue;
                 }
                 
                 // Convert path to waypoints
                 std::vector<sf::Vector2f> waypoints;
                 for (int vertex : path) {
                     waypoints.push_back(environmentGraph.getVertexPosition(vertex));
                 }
                 
                 // Set the path for the agent to follow
                 agent.setPath(waypoints);
                 
                 // Update stats text
                 if (fontLoaded) {
                     std::stringstream ss;
                     ss << "Algorithm: " << algorithmName << "\n"
                        << "Nodes Explored: " << nodesExplored << "\n"
                        << "Max Fringe Size: " << maxFringe << "\n"
                        << "Path Cost: " << std::fixed << std::setprecision(2) << pathCost << "\n"
                        << "Compute Time: " << std::fixed << std::setprecision(3) << computeTime << " ms";
                     statsText.setString(ss.str());
                 }
                 
                 std::cout << "Path found with " << algorithmName << ": " 
                           << path.size() << " vertices, "
                           << "cost = " << pathCost << ", "
                           << "nodes explored = " << nodesExplored << std::endl;
             }
         }
         
         // Update
         float deltaTime = clock.restart().asSeconds();
         agent.update(deltaTime);
         
         // Render
         window.clear(sf::Color::White);
         
         // Draw environment
         environment.draw(window);
         
         // Draw graph vertices if enabled
         if (showGraph) {
             for (const auto& vertex : graphVertices) {
                 window.draw(vertex);
             }
         }
         
         // Draw agent and path
         agent.draw(window);
         
         // Draw text
         if (fontLoaded) {
             window.draw(instructionText);
             window.draw(statsText);
         }
         
         window.display();
     }
     
     return 0;
 }