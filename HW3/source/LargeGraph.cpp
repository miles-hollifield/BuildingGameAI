/**
 * @file LargeGraph.cpp
 * @brief Creates a large random graph for pathfinding performance testing.
 *
 * Resources Used:
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - AI Tools: OpenAI's ChatGPT
 *
 * Author: Miles Hollifield
 * Date: 3/20/2025
 */

#include "../headers/Graph.h"
#include "../headers/Dijkstra.h"
#include "../headers/AStar.h"
#include "../headers/Heuristics.h"
#include <random>
#include <chrono>
#include <iostream>
#include <unordered_set>
#include <string>
#include <iomanip>
#include <vector>
#include <cmath>

/**
 * @brief Creates a large random graph with specified parameters.
 * @param numVertices Number of vertices in the graph.
 * @param avgEdgesPerVertex Average number of outgoing edges per vertex.
 * @param minWeight Minimum edge weight.
 * @param maxWeight Maximum edge weight.
 * @return A Graph object representing the large random graph.
 */
Graph createLargeGraph(int numVertices, int avgEdgesPerVertex, float minWeight, float maxWeight)
{
    // Create graph with specified vertices
    Graph largeGraph(numVertices);

    // Create random number generators
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> vertexDist(0, numVertices - 1);
    std::uniform_real_distribution<float> weightDist(minWeight, maxWeight);

    // Generate positions for heuristic calculations
    int gridSize = static_cast<int>(std::sqrt(numVertices)) + 1;
    std::vector<sf::Vector2f> positions;

    for (int i = 0; i < numVertices; i++)
    {
        int row = i / gridSize;
        int col = i % gridSize;
        positions.push_back(sf::Vector2f(col * 10.0f, row * 10.0f));
    }

    // Store positions in the graph
    largeGraph.setVertexPositions(positions);

    // Add random edges
    int totalEdges = numVertices * avgEdgesPerVertex;
    int edgesAdded = 0;

    std::cout << "\nGenerating " << totalEdges << " edges for " << numVertices << " vertices..." << std::endl;

    // Using sets to avoid duplicate edges
    std::vector<std::unordered_set<int>> adjacency(numVertices);

    while (edgesAdded < totalEdges)
    {
        int from = vertexDist(generator);
        int to = vertexDist(generator);

        // Avoid self-loops and duplicate edges
        if (from != to && adjacency[from].find(to) == adjacency[from].end())
        {
            float weight = weightDist(generator);
            largeGraph.addEdge(from, to, weight);
            adjacency[from].insert(to);
            edgesAdded++;
        }
    }

    std::cout << "Large graph generation complete: " << numVertices << " vertices, "
              << edgesAdded << " edges" << std::endl;

    return largeGraph;
}

/**
 * @brief Run performance tests on graph algorithms.
 * @param graph The graph to test.
 * @param numTrials Number of trials to run.
 * @note OpenAI's ChatGPT was used to suggest a template for this function.
 * The following prompt was used: "Create a template C++ function that runs performance tests on my graph algorithms using SFML."
 * The response was modified to fit the context of the code.
 */
void runPerformanceTests(Graph &graph, int numTrials)
{
    // Create algorithms
    Dijkstra dijkstra;

    AStar astarEuclidean([](int current, int goal, const Graph &g)
                         { return Heuristics::euclidean(current, goal, g); });

    AStar astarManhattan([](int current, int goal, const Graph &g)
                         { return Heuristics::manhattan(current, goal, g); });

    AStar astarInadmissible([](int current, int goal, const Graph &g)
                            { return Heuristics::inadmissible(current, goal, g); });

    // Create random number generator for selecting vertices
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> vertexDist(0, graph.size() - 1);

    // Statistics
    struct AlgoStats
    {
        double avgTime = 0.0;
        double avgNodesExplored = 0.0;
        double avgMaxFringe = 0.0;
        double avgPathCost = 0.0;
        double avgPathLength = 0.0;
        double minTime = std::numeric_limits<double>::max();
        double maxTime = 0.0;
        int successCount = 0;
        int optimalCount = 0;
    };

    AlgoStats dijkstraStats, astarEuclideanStats, astarManhattanStats, astarInadmissibleStats;

    // Run trials
    std::cout << "=====================================================" << std::endl;
    std::cout << "PATHFINDING TEST RESULTS - LARGE RANDOM GRAPH" << std::endl;
    std::cout << "=====================================================" << std::endl;
    std::cout << "\nRunning " << numTrials << " pathfinding trials..." << std::endl;

    for (int trial = 0; trial < numTrials; trial++)
    {
        // Select random start and goal vertices
        int start = vertexDist(generator);
        int goal = vertexDist(generator);

        // Skip if start and goal are the same
        if (start == goal)
        {
            trial--;
            continue;
        }

        std::cout << "\nTrial " << (trial + 1) << ": Finding path from vertex "
                  << start << " to " << goal << std::endl;
        std::cout << "-----------------------------------------------------" << std::endl;

        // Test Dijkstra
        auto dijkstraStart = std::chrono::high_resolution_clock::now();
        std::vector<int> dijkstraPath = dijkstra.findPath(graph, start, goal);
        auto dijkstraEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> dijkstraTime = dijkstraEnd - dijkstraStart;

        // Record Dijkstra results
        std::cout << "Dijkstra:" << std::endl;
        if (!dijkstraPath.empty())
        {
            dijkstraStats.successCount++;
            dijkstraStats.avgTime += dijkstraTime.count();
            dijkstraStats.avgNodesExplored += dijkstra.getNodesExplored();
            dijkstraStats.avgMaxFringe += dijkstra.getMaxFringeSize();
            dijkstraStats.avgPathCost += dijkstra.getPathCost();
            dijkstraStats.avgPathLength += dijkstraPath.size();
            dijkstraStats.minTime = std::min(dijkstraStats.minTime, dijkstraTime.count());
            dijkstraStats.maxTime = std::max(dijkstraStats.maxTime, dijkstraTime.count());
            dijkstraStats.optimalCount++; // Dijkstra's paths are always optimal

            std::cout << "  Path found! Length: " << dijkstraPath.size()
                      << ", Cost: " << dijkstra.getPathCost() << std::endl;
            std::cout << "  Nodes explored: " << dijkstra.getNodesExplored()
                      << ", Max fringe: " << dijkstra.getMaxFringeSize() << std::endl;
            std::cout << "  Execution time: " << dijkstraTime.count() << " ms" << std::endl;
        }
        else
        {
            std::cout << "  No path found!" << std::endl;
        }

        // Test A* with Euclidean heuristic
        auto astarEuclideanStart = std::chrono::high_resolution_clock::now();
        std::vector<int> astarEuclideanPath = astarEuclidean.findPath(graph, start, goal);
        auto astarEuclideanEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> astarEuclideanTime =
            astarEuclideanEnd - astarEuclideanStart;

        // Record A* Euclidean results
        std::cout << "A* (Euclidean):" << std::endl;
        if (!astarEuclideanPath.empty())
        {
            astarEuclideanStats.successCount++;
            astarEuclideanStats.avgTime += astarEuclideanTime.count();
            astarEuclideanStats.avgNodesExplored += astarEuclidean.getNodesExplored();
            astarEuclideanStats.avgMaxFringe += astarEuclidean.getMaxFringeSize();
            astarEuclideanStats.avgPathCost += astarEuclidean.getPathCost();
            astarEuclideanStats.avgPathLength += astarEuclideanPath.size();
            astarEuclideanStats.minTime = std::min(astarEuclideanStats.minTime, astarEuclideanTime.count());
            astarEuclideanStats.maxTime = std::max(astarEuclideanStats.maxTime, astarEuclideanTime.count());

            // Check if path is optimal (same cost as Dijkstra)
            if (!dijkstraPath.empty() &&
                std::fabs(astarEuclidean.getPathCost() - dijkstra.getPathCost()) < 0.001f)
            {
                astarEuclideanStats.optimalCount++;
            }

            std::cout << "  Path found! Length: " << astarEuclideanPath.size()
                      << ", Cost: " << astarEuclidean.getPathCost() << std::endl;
            std::cout << "  Nodes explored: " << astarEuclidean.getNodesExplored()
                      << ", Max fringe: " << astarEuclidean.getMaxFringeSize() << std::endl;
            std::cout << "  Execution time: " << astarEuclideanTime.count() << " ms" << std::endl;

            // Compare to Dijkstra
            if (!dijkstraPath.empty())
            {
                double nodesExploredRatio = (double)astarEuclidean.getNodesExplored() / dijkstra.getNodesExplored() * 100.0;
                std::cout << "  Explored " << std::fixed << std::setprecision(2) << nodesExploredRatio
                          << "% of the nodes that Dijkstra explored" << std::endl;
                std::cout << "  Path is " << (std::fabs(astarEuclidean.getPathCost() - dijkstra.getPathCost()) < 0.001f ? "optimal" : "NOT optimal") << std::endl;
            }
        }
        else
        {
            std::cout << "  No path found!" << std::endl;
        }

        // Test A* with Manhattan heuristic
        auto astarManhattanStart = std::chrono::high_resolution_clock::now();
        std::vector<int> astarManhattanPath = astarManhattan.findPath(graph, start, goal);
        auto astarManhattanEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> astarManhattanTime =
            astarManhattanEnd - astarManhattanStart;

        // Record A* Manhattan results
        std::cout << "A* (Manhattan):" << std::endl;
        if (!astarManhattanPath.empty())
        {
            astarManhattanStats.successCount++;
            astarManhattanStats.avgTime += astarManhattanTime.count();
            astarManhattanStats.avgNodesExplored += astarManhattan.getNodesExplored();
            astarManhattanStats.avgMaxFringe += astarManhattan.getMaxFringeSize();
            astarManhattanStats.avgPathCost += astarManhattan.getPathCost();
            astarManhattanStats.avgPathLength += astarManhattanPath.size();
            astarManhattanStats.minTime = std::min(astarManhattanStats.minTime, astarManhattanTime.count());
            astarManhattanStats.maxTime = std::max(astarManhattanStats.maxTime, astarManhattanTime.count());

            // Check if path is optimal (same cost as Dijkstra)
            if (!dijkstraPath.empty() &&
                std::fabs(astarManhattan.getPathCost() - dijkstra.getPathCost()) < 0.001f)
            {
                astarManhattanStats.optimalCount++;
            }

            std::cout << "  Path found! Length: " << astarManhattanPath.size()
                      << ", Cost: " << astarManhattan.getPathCost() << std::endl;
            std::cout << "  Nodes explored: " << astarManhattan.getNodesExplored()
                      << ", Max fringe: " << astarManhattan.getMaxFringeSize() << std::endl;
            std::cout << "  Execution time: " << astarManhattanTime.count() << " ms" << std::endl;

            // Compare to Dijkstra
            if (!dijkstraPath.empty())
            {
                double nodesExploredRatio = (double)astarManhattan.getNodesExplored() / dijkstra.getNodesExplored() * 100.0;
                std::cout << "  Explored " << std::fixed << std::setprecision(2) << nodesExploredRatio
                          << "% of the nodes that Dijkstra explored" << std::endl;
                std::cout << "  Path is " << (std::fabs(astarManhattan.getPathCost() - dijkstra.getPathCost()) < 0.001f ? "optimal" : "NOT optimal") << std::endl;
            }
        }
        else
        {
            std::cout << "  No path found!" << std::endl;
        }

        // Test A* with Inadmissible heuristic
        auto astarInadmissibleStart = std::chrono::high_resolution_clock::now();
        std::vector<int> astarInadmissiblePath = astarInadmissible.findPath(graph, start, goal);
        auto astarInadmissibleEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> astarInadmissibleTime =
            astarInadmissibleEnd - astarInadmissibleStart;

        // Record A* Inadmissible results
        std::cout << "A* (Inadmissible):" << std::endl;
        if (!astarInadmissiblePath.empty())
        {
            astarInadmissibleStats.successCount++;
            astarInadmissibleStats.avgTime += astarInadmissibleTime.count();
            astarInadmissibleStats.avgNodesExplored += astarInadmissible.getNodesExplored();
            astarInadmissibleStats.avgMaxFringe += astarInadmissible.getMaxFringeSize();
            astarInadmissibleStats.avgPathCost += astarInadmissible.getPathCost();
            astarInadmissibleStats.avgPathLength += astarInadmissiblePath.size();
            astarInadmissibleStats.minTime = std::min(astarInadmissibleStats.minTime, astarInadmissibleTime.count());
            astarInadmissibleStats.maxTime = std::max(astarInadmissibleStats.maxTime, astarInadmissibleTime.count());

            // Check if path is optimal (same cost as Dijkstra)
            if (!dijkstraPath.empty() &&
                std::fabs(astarInadmissible.getPathCost() - dijkstra.getPathCost()) < 0.001f)
            {
                astarInadmissibleStats.optimalCount++;
            }

            std::cout << "  Path found! Length: " << astarInadmissiblePath.size()
                      << ", Cost: " << astarInadmissible.getPathCost() << std::endl;
            std::cout << "  Nodes explored: " << astarInadmissible.getNodesExplored()
                      << ", Max fringe: " << astarInadmissible.getMaxFringeSize() << std::endl;
            std::cout << "  Execution time: " << astarInadmissibleTime.count() << " ms" << std::endl;

            // Compare to Dijkstra
            if (!dijkstraPath.empty())
            {
                double nodesExploredRatio = (double)astarInadmissible.getNodesExplored() / dijkstra.getNodesExplored() * 100.0;
                std::cout << "  Explored " << std::fixed << std::setprecision(2) << nodesExploredRatio
                          << "% of the nodes that Dijkstra explored" << std::endl;
                std::cout << "  Path is " << (std::fabs(astarInadmissible.getPathCost() - dijkstra.getPathCost()) < 0.001f ? "optimal" : "NOT optimal") << std::endl;
            }
        }
        else
        {
            std::cout << "  No path found!" << std::endl;
        }
    }

    // Calculate averages
    if (dijkstraStats.successCount > 0)
    {
        dijkstraStats.avgTime /= dijkstraStats.successCount;
        dijkstraStats.avgNodesExplored /= dijkstraStats.successCount;
        dijkstraStats.avgMaxFringe /= dijkstraStats.successCount;
        dijkstraStats.avgPathCost /= dijkstraStats.successCount;
        dijkstraStats.avgPathLength /= dijkstraStats.successCount;
    }

    if (astarEuclideanStats.successCount > 0)
    {
        astarEuclideanStats.avgTime /= astarEuclideanStats.successCount;
        astarEuclideanStats.avgNodesExplored /= astarEuclideanStats.successCount;
        astarEuclideanStats.avgMaxFringe /= astarEuclideanStats.successCount;
        astarEuclideanStats.avgPathCost /= astarEuclideanStats.successCount;
        astarEuclideanStats.avgPathLength /= astarEuclideanStats.successCount;
    }

    if (astarManhattanStats.successCount > 0)
    {
        astarManhattanStats.avgTime /= astarManhattanStats.successCount;
        astarManhattanStats.avgNodesExplored /= astarManhattanStats.successCount;
        astarManhattanStats.avgMaxFringe /= astarManhattanStats.successCount;
        astarManhattanStats.avgPathCost /= astarManhattanStats.successCount;
        astarManhattanStats.avgPathLength /= astarManhattanStats.successCount;
    }

    if (astarInadmissibleStats.successCount > 0)
    {
        astarInadmissibleStats.avgTime /= astarInadmissibleStats.successCount;
        astarInadmissibleStats.avgNodesExplored /= astarInadmissibleStats.successCount;
        astarInadmissibleStats.avgMaxFringe /= astarInadmissibleStats.successCount;
        astarInadmissibleStats.avgPathCost /= astarInadmissibleStats.successCount;
        astarInadmissibleStats.avgPathLength /= astarInadmissibleStats.successCount;
    }

    // Print overall results
    std::cout << "\n=====================================================" << std::endl;
    std::cout << "SUMMARY RESULTS (" << numTrials << " trials)" << std::endl;
    std::cout << "=====================================================" << std::endl;

    // Success rate
    std::cout << "Success Rate:" << std::endl;
    std::cout << "  Dijkstra: " << dijkstraStats.successCount << "/" << numTrials
              << " (" << (100.0 * dijkstraStats.successCount / numTrials) << "%)" << std::endl;
    std::cout << "  A* (Euclidean): " << astarEuclideanStats.successCount << "/" << numTrials
              << " (" << (100.0 * astarEuclideanStats.successCount / numTrials) << "%)" << std::endl;
    std::cout << "  A* (Manhattan): " << astarManhattanStats.successCount << "/" << numTrials
              << " (" << (100.0 * astarManhattanStats.successCount / numTrials) << "%)" << std::endl;
    std::cout << "  A* (Inadmissible): " << astarInadmissibleStats.successCount << "/" << numTrials
              << " (" << (100.0 * astarInadmissibleStats.successCount / numTrials) << "%)" << std::endl;

    // Optimality
    std::cout << "\nPath Optimality (when a path was found):" << std::endl;
    std::cout << "  Dijkstra: " << dijkstraStats.optimalCount << "/" << dijkstraStats.successCount
              << " (" << (100.0 * dijkstraStats.optimalCount / dijkstraStats.successCount) << "%)" << std::endl;
    std::cout << "  A* (Euclidean): " << astarEuclideanStats.optimalCount << "/" << astarEuclideanStats.successCount
              << " (" << (100.0 * astarEuclideanStats.optimalCount / astarEuclideanStats.successCount) << "%)" << std::endl;
    std::cout << "  A* (Manhattan): " << astarManhattanStats.optimalCount << "/" << astarManhattanStats.successCount
              << " (" << (100.0 * astarManhattanStats.optimalCount / astarManhattanStats.successCount) << "%)" << std::endl;
    std::cout << "  A* (Inadmissible): " << astarInadmissibleStats.optimalCount << "/" << astarInadmissibleStats.successCount
              << " (" << (100.0 * astarInadmissibleStats.optimalCount / astarInadmissibleStats.successCount) << "%)" << std::endl;

    // Performance metrics
    std::cout << "\nAverage Nodes Explored:" << std::endl;
    std::cout << "  Dijkstra: " << std::fixed << std::setprecision(1) << dijkstraStats.avgNodesExplored << std::endl;
    std::cout << "  A* (Euclidean): " << std::fixed << std::setprecision(1) << astarEuclideanStats.avgNodesExplored
              << " (" << (100.0 * astarEuclideanStats.avgNodesExplored / dijkstraStats.avgNodesExplored) << "% of Dijkstra)" << std::endl;
    std::cout << "  A* (Manhattan): " << std::fixed << std::setprecision(1) << astarManhattanStats.avgNodesExplored
              << " (" << (100.0 * astarManhattanStats.avgNodesExplored / dijkstraStats.avgNodesExplored) << "% of Dijkstra)" << std::endl;
    std::cout << "  A* (Inadmissible): " << std::fixed << std::setprecision(1) << astarInadmissibleStats.avgNodesExplored
              << " (" << (100.0 * astarInadmissibleStats.avgNodesExplored / dijkstraStats.avgNodesExplored) << "% of Dijkstra)" << std::endl;

    std::cout << "\nAverage Max Fringe Size:" << std::endl;
    std::cout << "  Dijkstra: " << std::fixed << std::setprecision(1) << dijkstraStats.avgMaxFringe << std::endl;
    std::cout << "  A* (Euclidean): " << std::fixed << std::setprecision(1) << astarEuclideanStats.avgMaxFringe << std::endl;
    std::cout << "  A* (Manhattan): " << std::fixed << std::setprecision(1) << astarManhattanStats.avgMaxFringe << std::endl;
    std::cout << "  A* (Inadmissible): " << std::fixed << std::setprecision(1) << astarInadmissibleStats.avgMaxFringe << std::endl;

    std::cout << "\nExecution Time (milliseconds):" << std::endl;
    std::cout << "  Dijkstra: avg=" << std::fixed << std::setprecision(3) << dijkstraStats.avgTime
              << ", min=" << dijkstraStats.minTime << ", max=" << dijkstraStats.maxTime << std::endl;
    std::cout << "  A* (Euclidean): avg=" << std::fixed << std::setprecision(3) << astarEuclideanStats.avgTime
              << ", min=" << astarEuclideanStats.minTime << ", max=" << astarEuclideanStats.maxTime
              << " (" << (100.0 * astarEuclideanStats.avgTime / dijkstraStats.avgTime) << "% of Dijkstra)" << std::endl;
    std::cout << "  A* (Manhattan): avg=" << std::fixed << std::setprecision(3) << astarManhattanStats.avgTime
              << ", min=" << astarManhattanStats.minTime << ", max=" << astarManhattanStats.maxTime
              << " (" << (100.0 * astarManhattanStats.avgTime / dijkstraStats.avgTime) << "% of Dijkstra)" << std::endl;
    std::cout << "  A* (Inadmissible): avg=" << std::fixed << std::setprecision(3) << astarInadmissibleStats.avgTime
              << ", min=" << astarInadmissibleStats.minTime << ", max=" << astarInadmissibleStats.maxTime
              << " (" << (100.0 * astarInadmissibleStats.avgTime / dijkstraStats.avgTime) << "% of Dijkstra)" << std::endl;

    std::cout << "\nAverage Path Length (vertices):" << std::endl;
    std::cout << "  Dijkstra: " << std::fixed << std::setprecision(1) << dijkstraStats.avgPathLength << std::endl;
    std::cout << "  A* (Euclidean): " << std::fixed << std::setprecision(1) << astarEuclideanStats.avgPathLength << std::endl;
    std::cout << "  A* (Manhattan): " << std::fixed << std::setprecision(1) << astarManhattanStats.avgPathLength << std::endl;
    std::cout << "  A* (Inadmissible): " << std::fixed << std::setprecision(1) << astarInadmissibleStats.avgPathLength << std::endl;

    std::cout << "\nAverage Path Cost:" << std::endl;
    std::cout << "  Dijkstra: " << std::fixed << std::setprecision(2) << dijkstraStats.avgPathCost << std::endl;
    std::cout << "  A* (Euclidean): " << std::fixed << std::setprecision(2) << astarEuclideanStats.avgPathCost
              << " (+" << (astarEuclideanStats.avgPathCost - dijkstraStats.avgPathCost) << " vs Dijkstra)" << std::endl;
    std::cout << "  A* (Manhattan): " << std::fixed << std::setprecision(2) << astarManhattanStats.avgPathCost
              << " (+" << (astarManhattanStats.avgPathCost - dijkstraStats.avgPathCost) << " vs Dijkstra)" << std::endl;
    std::cout << "  A* (Inadmissible): " << std::fixed << std::setprecision(2) << astarInadmissibleStats.avgPathCost
              << " (+" << (astarInadmissibleStats.avgPathCost - dijkstraStats.avgPathCost) << " vs Dijkstra)" << std::endl;
}

/**
 * @brief Main function to create and test large graph.
 */
int main()
{
    // Parameters for large graph
    int numVertices = 20000;   // 20,000 vertices
    int avgEdgesPerVertex = 5; // 5 edges per vertex (100,000 total edges)
    float minWeight = 1.0f;    // Minimum edge weight
    float maxWeight = 10.0f;   // Maximum edge weight

    // Create the large graph
    Graph largeGraph(0);
    std::cout << "Creating large graph with " << numVertices << " vertices..." << std::endl;
    largeGraph = createLargeGraph(numVertices, avgEdgesPerVertex, minWeight, maxWeight);

    // Run performance tests
    int numTrials = 10; // Number of pathfinding trials
    std::cout << "\nRunning performance tests on large graph..." << std::endl;
    runPerformanceTests(largeGraph, numTrials);

    return 0;
}