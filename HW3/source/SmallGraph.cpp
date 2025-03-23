/**
 * @file SmallGraph.cpp
 * @brief Creates a small, meaningful graph for pathfinding experiments.
 *
 * This graph represents a simplified map of the NCSU campus with key buildings
 * and walkways between them.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - Font: Arial.ttf (downloaded from https://font.download/font/arial)
 * - AI Tools: OpenAI's ChatGPT
 *
 * Author: Miles Hollifield
 * Date: 3/20/2025
 */

#include "../headers/Graph.h"
#include "../headers/Dijkstra.h"
#include "../headers/AStar.h"
#include "../headers/Heuristics.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <chrono>
#include <iomanip>

/**
 * @brief Creates a graph representing a simplified map of NCSU campus.
 * @return Graph object representing the campus.
 */
Graph createCampusGraph()
{
    // Define our locations (vertices)
    const int NUM_LOCATIONS = 20;

    // Create the graph with the specified number of vertices
    Graph campusGraph(NUM_LOCATIONS);

    // Define positions for each vertex for visualization
    std::vector<sf::Vector2f> positions = {
        {100, 100}, // 0: Talley Student Union
        {200, 100}, // 1: D.H. Hill Library
        {300, 150}, // 2: SAS Hall
        {350, 100}, // 3: Cox Hall
        {400, 150}, // 4: Engineering Building I
        {450, 200}, // 5: Engineering Building II
        {500, 250}, // 6: Engineering Building III
        {250, 200}, // 7: Daniels Hall
        {200, 250}, // 8: Riddick Hall
        {150, 250}, // 9: Mann Hall
        {150, 300}, // 10: Broughton Hall
        {250, 300}, // 11: Burlington Labs
        {350, 300}, // 12: Textiles Complex
        {450, 300}, // 13: Centennial Campus Center
        {100, 350}, // 14: Reynolds Coliseum
        {200, 350}, // 15: Carmichael Gym
        {300, 350}, // 16: Talley Student Center
        {400, 350}, // 17: Hunt Library
        {150, 50},  // 18: Bell Tower
        {300, 50}   // 19: Court of Carolinas
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
    campusGraph.addEdge(2, 1, 3.0f); // To D.H. Hill Library
    campusGraph.addEdge(2, 3, 2.0f); // To Cox Hall
    campusGraph.addEdge(2, 7, 1.5f); // To Daniels Hall

    // Cox Hall connections
    campusGraph.addEdge(3, 2, 2.0f);  // To SAS Hall
    campusGraph.addEdge(3, 4, 1.5f);  // To Engineering Building I
    campusGraph.addEdge(3, 19, 2.0f); // To Court of Carolinas

    // Engineering Building I connections
    campusGraph.addEdge(4, 3, 1.5f); // To Cox Hall
    campusGraph.addEdge(4, 5, 1.0f); // To Engineering Building II
    campusGraph.addEdge(4, 7, 3.0f); // To Daniels Hall

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
    campusGraph.addEdge(7, 1, 2.5f); // To D.H. Hill Library
    campusGraph.addEdge(7, 2, 1.5f); // To SAS Hall
    campusGraph.addEdge(7, 4, 3.0f); // To Engineering Building I
    campusGraph.addEdge(7, 8, 1.0f); // To Riddick Hall

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

    return campusGraph;
}

/**
 * @brief Run tests with different pathfinding algorithms on the campus graph.
 * @param graph The graph to test on.
 * @param locationNames Names of the locations (for reporting).
 * @note OpenAI's ChatGPT was used to suggest a template for this function.
 * The following prompt was used: "Create a function to test pathfinding algorithms on a graph.
 * Include Dijkstra's algorithm and A* with the different heuristics I defined.
 * Report the results in a structured format."
 * The response was modified to fit the context of the code.
 */
void testPathfinding(const Graph &graph, const std::vector<std::string> &locationNames)
{
    // Create pathfinding algorithms
    Dijkstra dijkstra;

    // A* with Euclidean distance heuristic (admissible and consistent)
    AStar astarEuclidean([](int current, int goal, const Graph &g)
                         { return Heuristics::euclidean(current, goal, g); });

    // A* with Manhattan distance heuristic (admissible)
    AStar astarManhattan([](int current, int goal, const Graph &g)
                         { return Heuristics::manhattan(current, goal, g); });

    // A* with inadmissible heuristic (overestimates)
    AStar astarInadmissible([](int current, int goal, const Graph &g)
                            { return Heuristics::inadmissible(current, goal, g); });

    // Define test cases (source, destination)
    std::vector<std::pair<int, int>> testCases = {
        {0, 17}, // Talley Student Union to Hunt Library
        {18, 6}, // Bell Tower to Engineering Building III
        {14, 3}, // Reynolds Coliseum to Cox Hall
        {15, 9}, // Carmichael Gym to Mann Hall
        {5, 12}  // Engineering Building II to Textiles Complex
    };

    // Run tests for each test case
    std::cout << "=====================================================" << std::endl;
    std::cout << "PATHFINDING TEST RESULTS - NCSU CAMPUS GRAPH" << std::endl;
    std::cout << "=====================================================" << std::endl;

    for (const auto &testCase : testCases)
    {
        int source = testCase.first;
        int dest = testCase.second;

        std::cout << "\nTest: " << locationNames[source] << " to " << locationNames[dest] << std::endl;
        std::cout << "-----------------------------------------------------" << std::endl;

        // Run Dijkstra's algorithm
        auto startTime = std::chrono::high_resolution_clock::now();
        std::vector<int> dijkstraPath = dijkstra.findPath(graph, source, dest);
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> dijkstraTime = endTime - startTime;

        // Report Dijkstra results
        std::cout << "Dijkstra:" << std::endl;
        std::cout << "  Path length: " << dijkstraPath.size() << " vertices" << std::endl;
        std::cout << "  Path cost: " << dijkstra.getPathCost() << std::endl;
        std::cout << "  Nodes explored: " << dijkstra.getNodesExplored() << std::endl;
        std::cout << "  Max fringe size: " << dijkstra.getMaxFringeSize() << std::endl;
        std::cout << "  Execution time: " << dijkstraTime.count() << " ms" << std::endl;
        std::cout << "  Path: ";
        for (size_t i = 0; i < dijkstraPath.size(); i++)
        {
            std::cout << locationNames[dijkstraPath[i]];
            if (i < dijkstraPath.size() - 1)
            {
                std::cout << " -> ";
            }
        }
        std::cout << std::endl;

        // Run A* with Euclidean heuristic
        startTime = std::chrono::high_resolution_clock::now();
        std::vector<int> astarEuclideanPath = astarEuclidean.findPath(graph, source, dest);
        endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> astarEuclideanTime = endTime - startTime;

        // Report A* Euclidean results
        std::cout << "\nA* (Euclidean):" << std::endl;
        std::cout << "  Path length: " << astarEuclideanPath.size() << " vertices" << std::endl;
        std::cout << "  Path cost: " << astarEuclidean.getPathCost() << std::endl;
        std::cout << "  Nodes explored: " << astarEuclidean.getNodesExplored() << std::endl;
        std::cout << "  Max fringe size: " << astarEuclidean.getMaxFringeSize() << std::endl;
        std::cout << "  Execution time: " << astarEuclideanTime.count() << " ms" << std::endl;
        std::cout << "  Path: ";
        for (size_t i = 0; i < astarEuclideanPath.size(); i++)
        {
            std::cout << locationNames[astarEuclideanPath[i]];
            if (i < astarEuclideanPath.size() - 1)
            {
                std::cout << " -> ";
            }
        }
        std::cout << std::endl;

        // Run A* with Manhattan heuristic
        startTime = std::chrono::high_resolution_clock::now();
        std::vector<int> astarManhattanPath = astarManhattan.findPath(graph, source, dest);
        endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> astarManhattanTime = endTime - startTime;

        // Report A* Manhattan results
        std::cout << "\nA* (Manhattan):" << std::endl;
        std::cout << "  Path length: " << astarManhattanPath.size() << " vertices" << std::endl;
        std::cout << "  Path cost: " << astarManhattan.getPathCost() << std::endl;
        std::cout << "  Nodes explored: " << astarManhattan.getNodesExplored() << std::endl;
        std::cout << "  Max fringe size: " << astarManhattan.getMaxFringeSize() << std::endl;
        std::cout << "  Execution time: " << astarManhattanTime.count() << " ms" << std::endl;
        std::cout << "  Path: ";
        for (size_t i = 0; i < astarManhattanPath.size(); i++)
        {
            std::cout << locationNames[astarManhattanPath[i]];
            if (i < astarManhattanPath.size() - 1)
            {
                std::cout << " -> ";
            }
        }
        std::cout << std::endl;

        // Run A* with Inadmissible heuristic
        startTime = std::chrono::high_resolution_clock::now();
        std::vector<int> astarInadmissiblePath = astarInadmissible.findPath(graph, source, dest);
        endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> astarInadmissibleTime = endTime - startTime;

        // Report A* Inadmissible results
        std::cout << "\nA* (Inadmissible):" << std::endl;
        std::cout << "  Path length: " << astarInadmissiblePath.size() << " vertices" << std::endl;
        std::cout << "  Path cost: " << astarInadmissible.getPathCost() << std::endl;
        std::cout << "  Nodes explored: " << astarInadmissible.getNodesExplored() << std::endl;
        std::cout << "  Max fringe size: " << astarInadmissible.getMaxFringeSize() << std::endl;
        std::cout << "  Execution time: " << astarInadmissibleTime.count() << " ms" << std::endl;
        std::cout << "  Path: ";
        for (size_t i = 0; i < astarInadmissiblePath.size(); i++)
        {
            std::cout << locationNames[astarInadmissiblePath[i]];
            if (i < astarInadmissiblePath.size() - 1)
            {
                std::cout << " -> ";
            }
        }
        std::cout << std::endl;

        // Check if paths are optimal
        bool euclideanOptimal = (astarEuclidean.getPathCost() == dijkstra.getPathCost());
        bool manhattanOptimal = (astarManhattan.getPathCost() == dijkstra.getPathCost());
        bool inadmissibleOptimal = (astarInadmissible.getPathCost() == dijkstra.getPathCost());

        std::cout << "\nPath Optimality:" << std::endl;
        std::cout << "  A* (Euclidean): " << (euclideanOptimal ? "Optimal" : "NOT Optimal") << std::endl;
        std::cout << "  A* (Manhattan): " << (manhattanOptimal ? "Optimal" : "NOT Optimal") << std::endl;
        std::cout << "  A* (Inadmissible): " << (inadmissibleOptimal ? "Optimal" : "NOT Optimal") << std::endl;

        // Compare node exploration
        double euclideanEfficiency = (double)astarEuclidean.getNodesExplored() / dijkstra.getNodesExplored() * 100.0;
        double manhattanEfficiency = (double)astarManhattan.getNodesExplored() / dijkstra.getNodesExplored() * 100.0;
        double inadmissibleEfficiency = (double)astarInadmissible.getNodesExplored() / dijkstra.getNodesExplored() * 100.0;

        std::cout << "\nExploration Efficiency (% of Dijkstra's nodes explored):" << std::endl;
        std::cout << "  A* (Euclidean): " << euclideanEfficiency << "%" << std::endl;
        std::cout << "  A* (Manhattan): " << manhattanEfficiency << "%" << std::endl;
        std::cout << "  A* (Inadmissible): " << inadmissibleEfficiency << "%" << std::endl;

        std::cout << "-----------------------------------------------------" << std::endl;
    }
}

/**
 * @brief Visualize the campus graph using SFML.
 * @param graph The graph to visualize.
 * @param locationNames Vector of location names for each vertex.
 */
void visualizeCampusGraph(const Graph &graph, const std::vector<std::string> &locationNames)
{
    // Create window
    sf::RenderWindow window(sf::VideoMode(640, 480), "NCSU Campus Graph");

    // Create font for text display
    sf::Font font;
    bool fontLoaded = false;

    // Load font from file
    if (font.loadFromFile("ARIAL.ttf"))
    {
        fontLoaded = true;
    }

    // Create shapes for vertices and edges
    std::vector<sf::CircleShape> vertices;
    std::vector<sf::RectangleShape> edges;
    std::vector<sf::Text> labels;

    // Set up vertices
    for (int i = 0; i < graph.size(); i++)
    {
        sf::Vector2f pos = graph.getVertexPosition(i);

        // Create label text
        if (fontLoaded)
        {
            sf::Text label(locationNames[i], font, 12);
            label.setPosition(pos + sf::Vector2f(-30, 12));
            label.setFillColor(sf::Color::Black);
            labels.push_back(label);
        }

        // Create vertex circle
        sf::CircleShape vertex(10);
        vertex.setPosition(pos - sf::Vector2f(10, 10)); // Center circle on position
        vertex.setFillColor(sf::Color::Blue);
        vertices.push_back(vertex);
    }

    // Set up edges
    for (int i = 0; i < graph.size(); i++)
    {
        sf::Vector2f from = graph.getVertexPosition(i);

        for (const auto &edge : graph.getNeighbors(i))
        {
            int to_idx = edge.first;
            float weight = edge.second;
            sf::Vector2f to = graph.getVertexPosition(to_idx);

            // Calculate edge direction
            sf::Vector2f direction = to - from;
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            if (length > 0)
            {
                direction /= length;

                // Create a rectangle to represent the edge
                sf::RectangleShape edgeShape(sf::Vector2f(length - 20, 2)); // Subtract diameter of vertices
                edgeShape.setPosition(from + direction * 10.0f);
                edgeShape.setRotation(std::atan2(direction.y, direction.x) * 180 / 3.14159265f);
                edgeShape.setFillColor(sf::Color(100, 100, 100, 150));
                edges.push_back(edgeShape);
            }
        }
    }

    // Select source and destination for pathfinding visualization
    int sourceVertex = 0; // Talley Student Union
    int destVertex = 17;  // Hunt Library

    // Create instances of algorithms
    Dijkstra dijkstra;
    AStar astarEuclidean([](int current, int goal, const Graph &g)
                         { return Heuristics::euclidean(current, goal, g); });
    AStar astarInadmissible([](int current, int goal, const Graph &g)
                            { return Heuristics::inadmissible(current, goal, g); });

    // Find paths
    std::vector<int> dijkstraPath = dijkstra.findPath(graph, sourceVertex, destVertex);
    std::vector<int> astarPath = astarEuclidean.findPath(graph, sourceVertex, destVertex);
    std::vector<int> inadmissiblePath = astarInadmissible.findPath(graph, sourceVertex, destVertex);

    // Output performance for this path
    std::cout << "\nPath from " << locationNames[sourceVertex] << " to " << locationNames[destVertex] << ":" << std::endl;
    std::cout << "Dijkstra: Nodes explored = " << dijkstra.getNodesExplored()
              << ", Cost = " << dijkstra.getPathCost() << std::endl;
    std::cout << "A* (Euclidean): Nodes explored = " << astarEuclidean.getNodesExplored()
              << ", Cost = " << astarEuclidean.getPathCost() << std::endl;
    std::cout << "A* (Inadmissible): Nodes explored = " << astarInadmissible.getNodesExplored()
              << ", Cost = " << astarInadmissible.getPathCost() << std::endl;

    // Create visualization for paths
    sf::VertexArray dijkstraLine(sf::LinesStrip, dijkstraPath.size());
    for (size_t i = 0; i < dijkstraPath.size(); i++)
    {
        dijkstraLine[i].position = graph.getVertexPosition(dijkstraPath[i]);
        dijkstraLine[i].color = sf::Color::Red;
    }

    sf::VertexArray inadmissibleLine(sf::LinesStrip, inadmissiblePath.size());
    for (size_t i = 0; i < inadmissiblePath.size(); i++)
    {
        inadmissibleLine[i].position = graph.getVertexPosition(inadmissiblePath[i]);
        inadmissibleLine[i].color = sf::Color::Blue;
    }

    sf::VertexArray astarLine(sf::LinesStrip, astarPath.size());
    for (size_t i = 0; i < astarPath.size(); i++)
    {
        astarLine[i].position = graph.getVertexPosition(astarPath[i]);
        astarLine[i].color = sf::Color::Green;
    }

    // Highlight source and destination
    sf::CircleShape sourceMarker(12);
    sourceMarker.setPosition(graph.getVertexPosition(sourceVertex) - sf::Vector2f(12, 12));
    sourceMarker.setFillColor(sf::Color::Transparent);
    sourceMarker.setOutlineColor(sf::Color::Yellow);
    sourceMarker.setOutlineThickness(3);

    sf::CircleShape destMarker(12);
    destMarker.setPosition(graph.getVertexPosition(destVertex) - sf::Vector2f(12, 12));
    destMarker.setFillColor(sf::Color::Transparent);
    destMarker.setOutlineColor(sf::Color::Magenta);
    destMarker.setOutlineThickness(3);

    // Create path information text display
    sf::Text pathInfoText;
    if (fontLoaded)
    {
        pathInfoText.setFont(font);
        pathInfoText.setCharacterSize(12);
        pathInfoText.setFillColor(sf::Color::Black);
        pathInfoText.setPosition(10, 450); // Position at bottom of screen

        std::stringstream ss;
        ss << "Dijkstra (Nodes: " << dijkstra.getNodesExplored()
           << ", Cost: " << std::fixed << std::setprecision(1) << dijkstra.getPathCost() << ")   |   ";

        ss << "A* Euclidean (Nodes: " << astarEuclidean.getNodesExplored()
           << ", Cost: " << std::fixed << std::setprecision(1) << astarEuclidean.getPathCost() << ")   |   \n";

        ss << "A* Inadmissible (Nodes: " << astarInadmissible.getNodesExplored()
           << ", Cost: " << std::fixed << std::setprecision(1) << astarInadmissible.getPathCost() << ")";

        pathInfoText.setString(ss.str());
    }

    // Main rendering loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
            }
        }

        window.clear(sf::Color::White);

        // Draw edges first
        for (const auto &edge : edges)
        {
            window.draw(edge);
        }

        // Draw path lines
        window.draw(dijkstraLine);
        window.draw(astarLine);
        window.draw(inadmissibleLine);

        // Draw vertices
        for (const auto &vertex : vertices)
        {
            window.draw(vertex);
        }

        // Draw source and destination markers
        window.draw(sourceMarker);
        window.draw(destMarker);

        // Draw labels (just the node labels)
        if (fontLoaded)
        {
            for (const auto &label : labels)
            {
                window.draw(label);
            }
        }

        window.draw(pathInfoText);

        window.display();
    }
}

/**
 * @brief Creates location names for the campus graph.
 * @return Vector of location names.
 */
std::vector<std::string> createLocationNames()
{
    return {
        "Talley Student \nUnion",     // 0
        "D.H. Hill \nLibrary",        // 1
        "SAS Hall",                   // 2
        "Cox Hall",                   // 3
        "Engineering \nBuilding I",   // 4
        "Engineering \nBuilding II",  // 5
        "Engineering \nBuilding III", // 6
        "Daniels Hall",               // 7
        "Riddick \nHall",             // 8
        "Mann \nHall",                // 9
        "Broughton Hall",             // 10
        "Burlington Labs",            // 11
        "Textiles Complex",           // 12
        "Centennial \nCampus Center", // 13
        "Reynolds \nColiseum",        // 14
        "Carmichael Gym",             // 15
        "Talley \nStudent Center",    // 16
        "Hunt Library",               // 17
        "Bell Tower",                 // 18
        "Court of \nCarolinas"        // 19
    };
}

/**
 * @brief Main function for testing the campus graph.
 */
int main()
{
    // Create the graph
    std::cout << "Creating campus graph..." << std::endl;
    Graph campusGraph = createCampusGraph();

    // Create location names
    std::vector<std::string> locationNames = createLocationNames();

    // Run algorithm comparison tests
    std::cout << "Running pathfinding tests..." << std::endl;
    testPathfinding(campusGraph, locationNames);

    // Visualize the graph
    std::cout << "Visualizing campus graph..." << std::endl;
    visualizeCampusGraph(campusGraph, locationNames);

    return 0;
}