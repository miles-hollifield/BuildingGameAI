# CSC 584/484 Spring 2025 - Homework 3: Pathfinding and Path Following

## Overview
This project implements pathfinding algorithms (Dijkstra's Algorithm and A*) and integrates them with path following behaviors from Homework 2. The implementation includes:

- Graph representation with weighted edges
- Dijkstra's shortest path algorithm
- A* algorithm with multiple heuristic functions
- Indoor environment with rooms and obstacles
- Click-to-navigate functionality using pathfinding
- Path following using steering behaviors from HW2

## Project Structure

### Headers
- **Steering Behaviors (from HW2)**
  - `Kinematic.h` - Position, velocity, orientation, and rotation
  - `SteeringData.h` - Linear and angular acceleration structures
  - `SteeringBehavior.h` - Abstract base class for behaviors
  - `Arrive.h` - Smooth arrival behavior
  - `Align.h` - Orientation alignment behavior

- **Pathfinding (new for HW3)**
  - `Graph.h` - Weighted directed graph representation
  - `Pathfinder.h` - Abstract base class for pathfinding
  - `Dijkstra.h` - Dijkstra's algorithm implementation
  - `AStar.h` - A* algorithm implementation
  - `Heuristics.h` - Various heuristic functions for A*
  - `Environment.h` - Indoor environment representation
  - `PathFollower.h` - Agent that follows paths using steering behaviors

### Source Files
- `hw3.cpp` - Main application for pathfinding and path following
- `SmallGraph.cpp` - Creates and tests a small, meaningful graph (NCSU campus)
- `LargeGraph.cpp` - Creates and tests a large random graph
- `Makefile` - Build configuration

## Features

### Graphs
- **Small Graph**: Represents NCSU campus with 20 vertices (buildings) and 66 edges (walkways)
- **Large Graph**: Randomly generated with tens of thousands of vertices and edges

### Pathfinding Algorithms
- **Dijkstra's Algorithm**: Guaranteed to find the shortest path
- **A* Algorithm**: Optimizes search using heuristic guidance

### Heuristics
The project implements several different heuristic functions for the A* algorithm:

1. **Euclidean Distance (Admissible & Consistent)**
   - Direct straight-line distance between vertices
   - Never overestimates the true cost, ensuring optimal paths
   - Used as the default heuristic in the main application

2. **Manhattan Distance (Admissible for Grid-Based Graphs)**
   - Sum of horizontal and vertical distances
   - Works well in environments with orthogonal movement
   - Admissible in grid-based environments but can be inadmissible in general graphs

3. **Cluster Heuristic (Performance Optimization)**
   - Groups vertices into clusters to accelerate large graph navigation
   - Uses pre-computed distances between clusters
   - Falls back to Euclidean distance for vertices in the same cluster
   - Efficient for large graphs with natural clustering

4. **Inadmissible Heuristic**
   - Deliberately overestimates costs based on distance
   - Uses variable overestimation factors (1.5-2.0×) based on distance
   - Adds random variation for non-deterministic behavior
   - Sacrifices path optimality for potentially faster pathfinding
   - Used to demonstrate effects of inadmissibility on A* performance

5. **Directional Bias Heuristic (Inadmissible)**
   - Systematically applies different weights to horizontal vs. vertical movement
   - Vertical movement is penalized more heavily (2.0× vs 1.2× for horizontal)
   - Creates directionally biased paths that prefer horizontal movement
   - Demonstrates how directional preferences can be encoded in pathfinding

The heuristics are designed to showcase the trade-offs between optimality, efficiency, and path characteristics in A* pathfinding.

### Indoor Environment
- Multiple rooms connected by doorways
- Obstacles placed within rooms
- Graph-based navigation
- Click-to-navigate interface

### Path Following
- Arrive behavior for smooth approach to waypoints
- Align behavior for orientation matching
- Visual breadcrumb trail to show path

## Compilation
Run the following command in the terminal from the project directory:

```
make
```

This will produce three executables:
- `hw3` - Main pathfinding and path following application
- `small_graph` - Small graph visualization and testing
- `large_graph` - Large graph generation and performance testing

## Execution

### Main Application
```
./hw3
```
This opens the interactive pathfinding and path following demo.
- Left-click: Find and follow path using A* with Euclidean heuristic
- Right-click: Find and follow path using Dijkstra's algorithm
- R: Reset agent position
- ESC: Exit application

### Small Graph Test
```
./small_graph
```
This visualizes the small graph (NCSU campus map) and runs test paths.
- Red path: Dijkstra's algorithm
- Green path: A* with Euclidean heuristic
- Blue path: A* with inadmissible heuristic

### Large Graph Test
```
./large_graph
```
This generates a large random graph and runs performance comparisons between algorithms.

## Implementation Notes

### Small Graph
The small graph represents a simplified map of NCSU campus with buildings and walkways. Each vertex represents a building or landmark, and edges represent walkways with weights based on arbitrary walking times.

### Large Graph 
The large graph is randomly generated with tens of thousands of vertices and edges to stress-test the pathfinding algorithms and measure their scaling behavior.

### Environment
The indoor environment is divided into a grid where walkable cells become vertices in the navigation graph. Obstacles and walls block movement, and the agent uses pathfinding to plan routes around them.

## Author
Miles Hollifield