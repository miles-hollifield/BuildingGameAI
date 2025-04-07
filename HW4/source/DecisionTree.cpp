/**
 * @file DecisionTree.cpp
 * @brief Implementation of the Decision Tree classes.
 *
 * Author: Miles Hollifield
 * Date: 4/7/2025
 */

#include "headers/DecisionTree.h"
#include <cmath>
#include <random>
#include <iostream>

// EnvironmentState implementation
EnvironmentState::EnvironmentState(const Kinematic &character)
    : characterKinematic(character),
      position(character.position),
      velocity(character.velocity),
      speed(0.0f),
      distanceToNearestObstacle(1000.0f),
      currentRoom(0),
      reachedWaypoint(false),
      completedPath(false),
      pathBlocked(false)
{
    stateTimer.restart();
    update();
}

void EnvironmentState::update()
{
    // Update cached values based on current conditions
    position = characterKinematic.position;
    velocity = characterKinematic.velocity;
    speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);

    // Calculate other state values
    // Note: In a complete implementation, these would be calculated based
    // on actual environment data. For now, they're just placeholders.
}

bool EnvironmentState::isNearObstacle(float threshold) const
{
    return distanceToNearestObstacle < threshold;
}

bool EnvironmentState::isMovingFast(float threshold) const
{
    return speed > threshold;
}

bool EnvironmentState::isInRoom(int roomId) const
{
    return currentRoom == roomId;
}

float EnvironmentState::getDistanceToTarget(const sf::Vector2f &target) const
{
    float dx = target.x - position.x;
    float dy = target.y - position.y;
    return std::sqrt(dx * dx + dy * dy);
}

bool EnvironmentState::hasBeenInCurrentState(float seconds) const
{
    return stateTimer.getElapsedTime().asSeconds() >= seconds;
}

bool EnvironmentState::hasReachedWaypoint() const
{
    return reachedWaypoint;
}

bool EnvironmentState::hasCompletedPath() const
{
    return completedPath;
}

bool EnvironmentState::isPathBlocked() const
{
    return pathBlocked;
}

bool EnvironmentState::canSeeTarget(const sf::Vector2f &target) const
{
    // In a complete implementation, this would use raycasting or similar to check
    // line of sight between the character and the target
    return true; // Placeholder
}

// DecisionTree implementation
DecisionTree::DecisionTree(EnvironmentState &state)
    : environmentState(state), rootNode(nullptr)
{
}

void DecisionTree::setRootNode(std::shared_ptr<DecisionNode> root)
{
    rootNode = root;
}

std::string DecisionTree::makeDecision()
{
    if (!rootNode)
    {
        std::cerr << "Decision tree has no root node!" << std::endl;
        return "Idle"; // Default action if no tree is defined
    }

    return rootNode->makeDecision();
}

void DecisionTree::buildSampleTree(const std::vector<sf::Vector2f> &targets)
{
    // This is a sample decision tree for demonstration purposes
    // A real tree would likely be more complex and tailored to the specific game

    // Random number generator for choosing targets
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> targetDist(0, targets.size() - 1);

    // Create action nodes for different behaviors
    auto wanderAction = std::make_shared<ActionNode>("Wander");
    auto pathfindAction = std::make_shared<ActionNode>("Pathfind");
    auto arriveAction = std::make_shared<ActionNode>("Arrive");
    auto fleeAction = std::make_shared<ActionNode>("Flee");
    auto idleAction = std::make_shared<ActionNode>("Idle");

    // Create target selection nodes
    sf::Vector2f randomTarget1 = targets[targetDist(gen)];
    sf::Vector2f randomTarget2 = targets[targetDist(gen)];
    sf::Vector2f randomTarget3 = targets[targetDist(gen)];

    // Create condition functions for decision branches
    auto isNearObstacle = [this]()
    { return environmentState.isNearObstacle(); };
    auto isMovingFast = [this]()
    { return environmentState.isMovingFast(); };
    auto hasReachedWaypoint = [this]()
    { return environmentState.hasReachedWaypoint(); };
    auto hasCompletedPath = [this]()
    { return environmentState.hasCompletedPath(); };
    auto hasBeenIdleTooLong = [this]()
    { return environmentState.hasBeenInCurrentState(3.0f); };
    auto canSeeTarget1 = [this, randomTarget1]()
    { return environmentState.canSeeTarget(randomTarget1); };
    auto isPathBlocked = [this]()
    { return environmentState.isPathBlocked(); };

    // Build the decision tree from bottom up
    // Level 3 (lowest level) decisions
    auto pathOrWander = std::make_shared<DecisionBranch>(
        isPathBlocked,
        wanderAction,
        pathfindAction,
        "Is path blocked?");

    auto fleeOrPathfind = std::make_shared<DecisionBranch>(
        isNearObstacle,
        fleeAction,
        pathfindAction,
        "Is near obstacle?");

    // Level 2 decisions
    auto wanderOrArrive = std::make_shared<DecisionBranch>(
        canSeeTarget1,
        arriveAction,
        wanderAction,
        "Can see target?");

    auto idleOrWander = std::make_shared<DecisionBranch>(
        hasBeenIdleTooLong,
        wanderOrArrive,
        idleAction,
        "Has been idle too long?");

    // Level 1 (highest level) decisions
    auto completedOrOnPath = std::make_shared<DecisionBranch>(
        hasCompletedPath,
        idleOrWander,
        pathOrWander,
        "Has completed path?");

    // Root decision
    auto rootDecision = std::make_shared<DecisionBranch>(
        isMovingFast,
        fleeOrPathfind,
        completedOrOnPath,
        "Is moving fast?");

    // Set the root node
    setRootNode(rootDecision);
}