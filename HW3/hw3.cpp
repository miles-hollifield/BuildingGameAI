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
    env.addRoom(sf::FloatRect(30, 30, 580, 420));
    
    // Add walls to define rooms with openings for doorways
    
    // Top horizontal divider with doorway gap
    env.addObstacle(sf::FloatRect(30, 170, 250, 10));  // Left section
    env.addObstacle(sf::FloatRect(330, 170, 280, 10)); // Right section
    
    // Vertical divider with two doorway gaps
    env.addObstacle(sf::FloatRect(330, 30, 10, 90));   // Top section
    env.addObstacle(sf::FloatRect(330, 210, 10, 120)); // Middle section
    env.addObstacle(sf::FloatRect(330, 370, 10, 80));  // Bottom section
    
    // Left vertical divider with doorway
    env.addObstacle(sf::FloatRect(170, 170, 10, 120)); // Upper section
    env.addObstacle(sf::FloatRect(170, 330, 10, 120)); // Lower section
    
    // Right vertical divider with doorway
    env.addObstacle(sf::FloatRect(490, 170, 10, 120)); // Upper section
    env.addObstacle(sf::FloatRect(490, 330, 10, 120)); // Lower section
    
    // Bottom horizontal divider with doorway gaps
    env.addObstacle(sf::FloatRect(30, 330, 90, 10));   // Left section
    env.addObstacle(sf::FloatRect(210, 330, 80, 10));  // Middle-left section
    env.addObstacle(sf::FloatRect(370, 330, 80, 10));  // Middle-right section
    env.addObstacle(sf::FloatRect(530, 330, 80, 10));  // Right section
    
    // Add obstacles within rooms
    // Top-left room obstacles
    //env.addObstacle(sf::FloatRect(90, 80, 30, 30));
    
    // Top-right room obstacles
    env.addObstacle(sf::FloatRect(410, 80, 30, 40));
    
    // Middle-left room obstacles
    env.addObstacle(sf::FloatRect(80, 240, 40, 30));
    
    // Middle-center room obstacles
    env.addObstacle(sf::FloatRect(260, 240, 30, 50));
    
    // Middle-right room obstacles
    env.addObstacle(sf::FloatRect(400, 240, 50, 40));
    
    // Bottom row room obstacles
    env.addObstacle(sf::FloatRect(120, 380, 30, 30));
    env.addObstacle(sf::FloatRect(270, 380, 25, 35));
    env.addObstacle(sf::FloatRect(530, 380, 40, 25));
    
    return env;
}
 
 int main() {
     // Create window
     int windowWidth = 640;
     int windowHeight = 480;
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
        instructionText.setString("Left-click: A* | Right-click: Dijkstra | R: Reset agent");
        instructionText.setCharacterSize(10);  // Smaller text to fit in one line
        instructionText.setFillColor(sf::Color::Black);
        instructionText.setPosition(5, 5);  // At the very top
    }
     
     // Text for path statistics
     sf::Text statsText;
     if (fontLoaded) {
         statsText.setFont(font);
         statsText.setCharacterSize(12);
         statsText.setFillColor(sf::Color::Black);
         statsText.setPosition(5, windowHeight - 20);
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
                    std::cout << "Cannot navigate - clicked on obstacle" << std::endl;
                    continue;
                }
                
                // Get current agent position
                sf::Vector2f agentPos = agent.getPosition();
                
                // Convert positions to graph vertices
                int startVertex = environment.pointToVertex(agentPos);
                int goalVertex = environment.pointToVertex(mousePos);
                
                std::cout << "Agent position: (" << agentPos.x << "," << agentPos.y << ") -> vertex " << startVertex << std::endl;
                std::cout << "Goal position: (" << mousePos.x << "," << mousePos.y << ") -> vertex " << goalVertex << std::endl;
                
                // Skip if vertices are the same
                if (startVertex == goalVertex) {
                    std::cout << "Start and goal vertices are the same - skipping" << std::endl;
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
                    std::cout << "No path found from vertex " << startVertex << " to " << goalVertex << std::endl;
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
                
                std::cout << "Path found with " << waypoints.size() << " waypoints:" << std::endl;
                for (size_t i = 0; i < waypoints.size(); i++) {
                    std::cout << "  " << i << ": (" << waypoints[i].x << "," << waypoints[i].y << ")" << std::endl;
                }
                
                // Set the path for the agent to follow
                agent.setPath(waypoints);
                
                // Update stats text
                if (fontLoaded) {
                    std::stringstream ss;
                    ss << "Algo: " << algorithmName 
                       << " | Nodes: " << nodesExplored 
                       << " | Fringe: " << maxFringe 
                       << " | Cost: " << std::fixed << std::setprecision(1) << pathCost 
                       << " | Time: " << std::fixed << std::setprecision(1) << computeTime << "ms";
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