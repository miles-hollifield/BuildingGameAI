# CSC 584/484 Homework 4: Decision Trees, Behavior Trees, and Learning

## Overview
This project demonstrates the implementation of decision trees, behavior trees, and decision tree learning for AI in games. The application builds on the movement and pathfinding code from Homework 2 and 3, adding higher-level decision making.

## Features
- **Decision Trees**: Manual implementation of decision trees for agent behavior
- **Behavior Trees**: Implementation of behavior trees with various node types
- **Decision Tree Learning**: Learning decision trees from behavior data
- **Comparative Analysis**: Tools to compare the effectiveness of the different approaches

## Requirements
- C++17 compatible compiler
- SFML 2.5 or higher
- Linux or macOS

## Project Structure
- `headers/` - Header files
  - `DecisionTree.h` - Decision tree implementation
  - `BehaviorTree.h` - Behavior tree implementation
  - `Monster.h` - Monster entity controlled by AI
  - `DTLearning.h` - Decision tree learning algorithm
  - Various headers from HW2 and HW3
- `source/` - Source files
  - `DecisionTree.cpp`
  - `BehaviorTree.cpp`
  - `Monster.cpp`
  - `DTLearning.cpp`
  - Various source files from HW2 and HW3
- `hw4.cpp` - Main application
- `Makefile` - Build configuration

## Building the Project
To build the project, run:
```bash
make
```

This will compile all source files and create the executable `hw4`.

## Running the Project
To run the project, execute:
```bash
./hw4
```

## Controls
- **Left-click**: Set player destination
- **R**: Reset positions
- **1**: Record behavior tree data (toggle)
- **2**: Learn decision tree from recorded data
- **3**: Toggle monster type (behavior tree, decision tree, or both)
- **ESC**: Exit application

## Implementation Details

### Decision Tree
The decision tree implementation allows agents to make decisions based on environmental conditions such as:
- Distance to the player
- Current velocity
- Whether obstacles are nearby
- Whether the player is visible
- Time spent in current state

Actions controlled by the decision tree include:
- Path finding to the player
- Wandering
- Dancing
- Idling

### Behavior Tree
The behavior tree implementation includes several node types:
- **Sequence**: Executes children in order until one fails
- **Selector**: Tries children in order until one succeeds
- **Inverter**: Inverts the result of its child
- **Repeater**: Repeats its child a specified number of times
- **Parallel**: Executes all children simultaneously

The monster's behavior tree includes logic for:
- Chasing the player when visible
- Wandering when no other actions are available
- Occasionally performing a dance

### Decision Tree Learning
The decision tree learning implementation uses the ID3 algorithm to learn a decision tree from recorded behavior data. The algorithm:
1. Calculates entropy and information gain for each attribute
2. Selects the attribute with the highest information gain
3. Recursively builds subtrees for each value of the selected attribute

## Author
Miles Hollifield