# CSC 584/484 Homework 4: Decision Trees, Behavior Trees, and Learning

## Overview
This project implements decision trees, behavior trees, and decision tree learning for game AI, building upon previous movement and pathfinding work. It features autonomous agents making decisions in an indoor environment with multiple rooms.

## Features
- Decision trees for autonomous agent behavior
- Behavior trees with various node types
- Decision tree learning from behavior data
- Comparative performance analysis

## Building and Running
Run the following commands in the terminal from the project directory to produce and run the `hw4` executable:
```
make        # Build the project
./hw4       # Run the application
make clean  # Clean build files
```

## Controls
- **R**: Reset positions
- **1**: Record behavior tree data (toggle)
- **2**: Learn decision tree from recorded data
- **3**: Toggle monsters (behavior tree, decision tree, or both)
- **ESC**: Exit application

## Implementation
- **Decision Trees**: Provides autonomous behavior based on environment state (distance, velocity, obstacles, visibility)
- **Behavior Trees**: Includes sequence nodes, selector nodes, decorators, random selectors, and parallel nodes
- **Decision Tree Learning**: Uses ID3 algorithm to learn from recorded behavior data

## Experiment
The application allows recording behavior tree actions, learning a decision tree from this data, and comparing performance between the original and learned behaviors using metrics like catch count and time.

## Author
Miles Hollifield