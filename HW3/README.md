# CSC 584/484 Spring 2025 - Homework 3: Pathfinding and Path Following

## Overview
This project implements pathfinding algorithms (Dijkstra's Algorithm and A*) and integrates them with path following behaviors from Homework 2. The implementation includes:

- Graph representation with weighted edges
- Dijkstra's shortest path algorithm
- A* algorithm with multiple heuristic functions
- Indoor environment with rooms and obstacles
- Click-to-navigate functionality using pathfinding
- Path following using steering behaviors from HW2

## File Structure

- **headers/**
  - `Graph.h` - Graph representation
  - `Pathfinder.h` - Base class for pathfinding algorithms
  - `Dijkstra.h` - Dijkstra's algorithm implementation
  - `AStar.h` - A* algorithm implementation
  - `Heuristics.h` - Various heuristic functions for A*
  - `Environment.h` - Indoor environment representation
  - `PathFollower.h` - Path following agent

- **hw3.cpp** - Main application
- **SmallGraph.cpp** - Creates and tests a small, meaningful graph (NCSU campus)
- **LargeGraph.cpp** - Creates and tests a large random graph
- **Makefile** - Build configuration

## Requirements
- SFML 2.5 or newer
- C++17 compatible compiler
- Your HW2 steering behavior code

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
- Middle-click: Find and follow path using A* with inadmissible heuristic

### Small Graph Test
```
./small_graph
```
This visualizes the small graph (NCSU campus map) and runs some test paths.

### Large Graph Test
```
./large_graph
```
This generates a large random graph and runs performance comparisons between the algorithms.

## Graphs

### Small Graph
The small graph represents a simplified map of the NCSU campus with key buildings and walkways between them. Each vertex represents a building or landmark, and edges represent walkways with weights that correspond to approximate walking times.

### Large Graph
The large graph is randomly generated with tens of thousands of vertices and edges. It's designed to stress-test the pathfinding algorithms and measure their performance at scale.

## Heuristics for A*

1. **Euclidean Distance** (Admissible and Consistent)
   - Straight-line distance between vertices
   - Never overestimates the true cost

2. **Manhattan Distance** (Admissible and Consistent for grid-based environments)
   - Sum of horizontal and vertical distances
   - Works well in environments with orthogonal movement

3. **Inadmissible Heuristic**
   - Deliberately overestimates costs
   - Sacrifices optimality for potentially faster computation

## Notes
- The indoor environment is divided into a grid, with each grid cell becoming a vertex in the graph.
- Path following uses the Arrive and Align steering behaviors from HW2.
- Breadcrumbs trace the agent's movement to visualize the followed path.

## Author
Miles Hollifield