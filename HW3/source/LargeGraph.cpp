/**
 * @file LargeGraph.cpp
 * @brief Creates a large random graph for pathfinding performance testing.
 *
 * Author: Miles Hollifield
 * Date: 3/20/2025
 */

 #include "headers/Graph.h"
 #include <random>
 #include <chrono>
 #include <iostream>
 #include <unordered_set>
 #include <string>
 #include <cmath>
 
 /**
  * @brief Creates a large random graph with specified parameters.
  * @param numVertices Number of vertices in the graph.
  * @param avgEdgesPerVertex Average number of outgoing edges per vertex.
  * @param minWeight Minimum edge weight.
  * @param maxWeight Maximum edge weight.
  * @return A Graph object representing the large random graph.
  */
 Graph createLargeGraph(int numVertices, int avgEdgesPerVertex, float minWeight, float maxWeight) {
     // Create graph with specified vertices
     Graph largeGraph(numVertices);
     
     // Create random number generators
     unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
     std::default_random_engine generator(seed);
     std::uniform_int_distribution<int> vertexDist(0, numVertices - 1);
     std::uniform_real_distribution<float> weightDist(minWeight, maxWeight);
     
     // Generate positions for heuristic calculations (in a virtual 2D space)
     int gridSize = static_cast<int>(std::sqrt(numVertices)) + 1;
     std::vector<sf::Vector2f> positions;
     
     for (int i = 0; i < numVertices; i++) {
         int row = i / gridSize;
         int col = i % gridSize;
         positions.push_back(sf::Vector2f(col * 10.0f, row * 10.0f));
     }
     
     // Store positions in the graph
     largeGraph.setVertexPositions(positions);
     
     // Add random edges
     int totalEdges = numVertices * avgEdgesPerVertex;
     int edgesAdded = 0;
     
     std::cout << "Generating " << totalEdges << " edges for " << numVertices << " vertices..." << std::endl;
     
     // Using sets to avoid duplicate edges
     std::vector<std::unordered_set<int>> adjacency(numVertices);
     
     // Progress tracking
     int progressStep = totalEdges / 20; // Update progress every 5%
     int nextProgressUpdate = progressStep;
     
     while (edgesAdded < totalEdges) {
         int from = vertexDist(generator);
         int to = vertexDist(generator);
         
         // Avoid self-loops and duplicate edges
         if (from != to && adjacency[from].find(to) == adjacency[from].end()) {
             float weight = weightDist(generator);
             largeGraph.addEdge(from, to, weight);
             adjacency[from].insert(to);
             edgesAdded++;
             
             // Update progress
             if (edgesAdded >= nextProgressUpdate) {
                 std::cout << "Progress: " << (edgesAdded * 100 / totalEdges) << "% complete" << std::endl;
                 nextProgressUpdate += progressStep;
             }
         }
     }
     
     std::cout << "Large graph generation complete: " << numVertices << " vertices, " 
               << edgesAdded << " edges" << std::endl;
     
     // Save graph to file for later use
     largeGraph.saveToFile("large_graph.dat");
     
     return largeGraph;
 }
 
 /**
  * @brief Run performance tests on graph algorithms.
  * @param graph The graph to test.
  * @param numTrials Number of trials to run.
  * @param isLargeGraph Whether this is the large graph (affects visualization).
  */
 void runPerformanceTests(Graph& graph, int numTrials, bool isLargeGraph) {
     // Create algorithms
     Dijkstra dijkstra;
     
     AStar astarEuclidean([](int current, int goal, const Graph& g) {
         return Heuristics::euclidean(current, goal, g);
     });
     
     AStar astarManhattan([](int current, int goal, const Graph& g) {
         return Heuristics::manhattan(current, goal, g);
     });
     
     AStar astarInadmissible([](int current, int goal, const Graph& g) {
         return Heuristics::inadmissible(current, goal, g);
     });
     
     // Create random number generator for selecting vertices
     unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
     std::default_random_engine generator(seed);
     std::uniform_int_distribution<int> vertexDist(0, graph.size() - 1);
     
     // Statistics
     struct AlgoStats {
         double avgTime = 0.0;
         double avgNodesExplored = 0.0;
         double avgMaxFringe = 0.0;
         double avgPathCost = 0.0;
         double avgPathLength = 0.0;
         int successCount = 0;
     };
     
     AlgoStats dijkstraStats, astarEuclideanStats, astarManhattanStats, astarInadmissibleStats;
     
     // Run trials
     std::cout << "Running " << numTrials << " pathfinding trials..." << std::endl;
     
     for (int trial = 0; trial < numTrials; trial++) {
         // Select random start and goal vertices
         int start = vertexDist(generator);
         int goal = vertexDist(generator);
         
         // Skip if start and goal are the same
         if (start == goal) {
             trial--;
             continue;
         }
         
         std::cout << "Trial " << (trial + 1) << ": Finding path from vertex " 
                   << start << " to " << goal << std::endl;
         
         // Test Dijkstra
         auto dijkstraStart = std::chrono::high_resolution_clock::now();
         std::vector<int> dijkstraPath = dijkstra.findPath(graph, start, goal);
         auto dijkstraEnd = std::chrono::high_resolution_clock::now();
         std::chrono::duration<double, std::milli> dijkstraTime = dijkstraEnd - dijkstraStart;
         
         if (!dijkstraPath.empty()) {
             dijkstraStats.successCount++;
             dijkstraStats.avgTime += dijkstraTime.count();
             dijkstraStats.avgNodesExplored += dijkstra.getNodesExplored();
             dijkstraStats.avgMaxFringe += dijkstra.getMaxFringeSize();
             dijkstraStats.avgPathCost += dijkstra.getPathCost();
             dijkstraStats.avgPathLength += dijkstraPath.size();
         }
         
         // Test A* with Euclidean heuristic
         auto astarEuclideanStart = std::chrono::high_resolution_clock::now();
         std::vector<int> astarEuclideanPath = astarEuclidean.findPath(graph, start, goal);
         auto astarEuclideanEnd = std::chrono::high_resolution_clock::now();
         std::chrono::duration<double, std::milli> astarEuclideanTime = 
             astarEuclideanEnd - astarEuclideanStart;
         
         if (!astarEuclideanPath.empty()) {
             astarEuclideanStats.successCount++;
             astarEuclideanStats.avgTime += astarEuclideanTime.count();
             astarEuclideanStats.avgNodesExplored += astarEuclidean.getNodesExplored();
             astarEuclideanStats.avgMaxFringe += astarEuclidean.getMaxFringeSize();
             astarEuclideanStats.avgPathCost += astarEuclidean.getPathCost();
             astarEuclideanStats.avgPathLength += astarEuclideanPath.size();
         }
         
         // Test A* with Manhattan heuristic
         auto astarManhattanStart = std::chrono::high_resolution_clock::now();
         std::vector<int> astarManhattanPath = astarManhattan.findPath(graph, start, goal);
         auto astarManhattanEnd = std::chrono::high_resolution_clock::now();
         std::chrono::duration<double, std::milli> astarManhattanTime = 
             astarManhattanEnd - astarManhattanStart;
         
         if (!astarManhattanPath.empty()) {
             astarManhattanStats.successCount++;
             astarManhattanStats.avgTime += astarManhattanTime.count();
             astarManhattanStats.avgNodesExplored += astarManhattan.getNodesExplored();
             astarManhattanStats.avgMaxFringe += astarManhattan.getMaxFringeSize();
             astarManhattanStats.avgPathCost += astarManhattan.getPathCost();
             astarManhattanStats.avgPathLength += astarManhattanPath.size();
         }
         
         // Test A* with Inadmissible heuristic
         auto astarInadmissibleStart = std::chrono::high_resolution_clock::now();
         std::vector<int> astarInadmissiblePath = astarInadmissible.findPath(graph, start, goal);
         auto astarInadmissibleEnd = std::chrono::high_resolution_clock::now();
         std::chrono::duration<double, std::milli> astarInadmissibleTime = 
             astarInadmissibleEnd - astarInadmissibleStart;
         
         if (!astarInadmissiblePath.empty()) {
             astarInadmissibleStats.successCount++;
             astarInadmissibleStats.avgTime += astarInadmissibleTime.count();
             astarInadmissibleStats.avgNodesExplored += astarInadmissible.getNodesExplored();
             astarInadmissibleStats.avgMaxFringe += astarInadmissible.getMaxFringeSize();
             astarInadmissibleStats.avgPathCost += astarInadmissible.getPathCost();
             astarInadmissibleStats.avgPathLength += astarInadmissiblePath.size();
         }
         
         // Print results for this trial
         std::cout << "  Dijkstra: " << dijkstraTime.count() << "ms, " 
                  << dijkstra.getNodesExplored() << " nodes explored" << std::endl;
         std::cout << "  A* (Euclidean): " << astarEuclideanTime.count() << "ms, " 
                  << astarEuclidean.getNodesExplored() << " nodes explored" << std::endl;
         std::cout << "  A* (Manhattan): " << astarManhattanTime.count() << "ms, " 
                  << astarManhattan.getNodesExplored() << " nodes explored" << std::endl;
         std::cout << "  A* (Inadmissible): " << astarInadmissibleTime.count() << "ms, " 
                  << astarInadmissible.getNodesExplored() << " nodes explored" << std::endl;
     }
     
     // Calculate averages
     if (dijkstraStats.successCount > 0) {
         dijkstraStats.avgTime /= dijkstraStats.successCount;
         dijkstraStats.avgNodesExplored /= dijkstraStats.successCount;
         dijkstraStats.avgMaxFringe /= dijkstraStats.successCount;
         dijkstraStats.avgPathCost /= dijkstraStats.successCount;
         dijkstraStats.avgPathLength /= dijkstraStats.successCount;
     }
     
     if (astarEuclideanStats.successCount > 0) {
         astarEuclideanStats.avgTime /= astarEuclideanStats.successCount;
         astarEuclideanStats.avgNodesExplored /= astarEuclideanStats.successCount;
         astarEuclideanStats.avgMaxFringe /= astarEuclideanStats.successCount;
         astarEuclideanStats.avgPathCost /= astarEuclideanStats.successCount;
         astarEuclideanStats.avgPathLength /= astarEuclideanStats.successCount;
     }
     
     if (astarManhattanStats.successCount > 0) {
         astarManhattanStats.avgTime /= astarManhattanStats.successCount;
         astarManhattanStats.avgNodesExplored /= astarManhattanStats.successCount;
         astarManhattanStats.avgMaxFringe /= astarManhattanStats.successCount;
         astarManhattanStats.avgPathCost /= astarManhattanStats.successCount;
         astarManhattanStats.avgPathLength /= astarManhattanStats.successCount;
     }
     
     if (astarInadmissibleStats.successCount > 0) {
         astarInadmissibleStats.avgTime /= astarInadmissibleStats.successCount;
         astarInadmissibleStats.avgNodesExplored /= astarInadmissibleStats.successCount;
         astarInadmissibleStats.avgMaxFringe /= astarInadmissibleStats.successCount;
         astarInadmissibleStats.avgPathCost /= astarInadmissibleStats.successCount;
         astarInadmissibleStats.avgPathLength /= astarInadmissibleStats.successCount;
     }
     
     // Print overall results
     std::cout << "\nOverall Results (" << numTrials << " trials):" << std::endl;
     std::cout << "------------------------------------------------------" << std::endl;
     std::cout << "Algorithm | Success | Time (ms) | Nodes | Fringe | Cost | Length" << std::endl;
     std::cout << "------------------------------------------------------" << std::endl;
     
     printf("Dijkstra  | %7d | %9.2f | %5.0f | %6.0f | %4.1f | %6.1f\n", 
            dijkstraStats.successCount, dijkstraStats.avgTime, 
            dijkstraStats.avgNodesExplored, dijkstraStats.avgMaxFringe,
            dijkstraStats.avgPathCost, dijkstraStats.avgPathLength);
     
     printf("A*(Euclid)| %7d | %9.2f | %5.0f | %6.0f | %4.1f | %6.1f\n", 
            astarEuclideanStats.successCount, astarEuclideanStats.avgTime, 
            astarEuclideanStats.avgNodesExplored, astarEuclideanStats.avgMaxFringe,
            astarEuclideanStats.avgPathCost, astarEuclideanStats.avgPathLength);
     
     printf("A*(Manhat)| %7d | %9.2f | %5.0f | %6.0f | %4.1f | %6.1f\n", 
            astarManhattanStats.successCount, astarManhattanStats.avgTime, 
            astarManhattanStats.avgNodesExplored, astarManhattanStats.avgMaxFringe,
            astarManhattanStats.avgPathCost, astarManhattanStats.avgPathLength);
     
     printf("A*(Inadm) | %7d | %9.2f | %5.0f | %6.0f | %4.1f | %6.1f\n", 
            astarInadmissibleStats.successCount, astarInadmissibleStats.avgTime, 
            astarInadmissibleStats.avgNodesExplored, astarInadmissibleStats.avgMaxFringe,
            astarInadmissibleStats.avgPathCost, astarInadmissibleStats.avgPathLength);
     
     std::cout << "------------------------------------------------------" << std::endl;
 }
 
 /**
  * @brief Main function to create and test large graph.
  * @return 0 on success.
  */
 int main() {
     // Parameters for large graph
     int numVertices = 20000;  // 20,000 vertices
     int avgEdgesPerVertex = 5; // 5 edges per vertex
     float minWeight = 1.0f;    // Minimum edge weight
     float maxWeight = 10.0f;   // Maximum edge weight
     
     // Create the large graph
     std::cout << "Creating large graph with " << numVertices << " vertices..." << std::endl;
     Graph largeGraph = createLargeGraph(numVertices, avgEdgesPerVertex, minWeight, maxWeight);
     
     // Run performance tests
     int numTrials = 20; // Number of pathfinding trials
     std::cout << "\nRunning performance tests on large graph..." << std::endl;
     runPerformanceTests(largeGraph, numTrials, true);
     
     return 0;
 }