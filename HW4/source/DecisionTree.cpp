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
#include <sstream>
#include <iomanip>

// EnvironmentState implementation
EnvironmentState::EnvironmentState(const Kinematic &character, const Environment &env)
    : character(character),
      environment(env),
      position(character.position),
      velocity(character.velocity),
      speed(0.0f),
      distanceToNearestObstacle(1000.0f),
      currentRoom(0),
      reachedWaypoint(false),
      completedPath(false),
      pathBlocked(false),
      isIdle(true)
{
    stateTimer.restart();
    idleTimer.restart();
    update();
}

void EnvironmentState::update()
{
    // Update cached values based on current conditions
    position = character.position;
    velocity = character.velocity;
    speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    
    // Update idle state
    if (speed < 1.0f) {
        if (!isIdle) {
            isIdle = true;
            idleTimer.restart();
        }
    } else {
        isIdle = false;
    }

    // Calculate other state values
    findNearestObstacle();
    currentRoom = determineCurrentRoom();
    
    // Determine path status
    // Note: In a full implementation, these would be calculated based on path following state
    // from a PathFollower or similar class. For now, we'll use simplified assumptions.
    if (currentTarget != sf::Vector2f(0, 0)) {
        float distanceToTarget = getDistanceToTarget(currentTarget);
        reachedWaypoint = distanceToTarget < 20.0f;
        completedPath = reachedWaypoint; // Simplified for now
    }
}

void EnvironmentState::setTarget(const sf::Vector2f &target)
{
    currentTarget = target;
}

void EnvironmentState::resetStateTimer()
{
    stateTimer.restart();
}

void EnvironmentState::findNearestObstacle()
{
    // In a real implementation, this would scan nearby obstacles
    // For now, we'll use a simplified version checking in 8 directions
    
    const float MAX_CHECK_DISTANCE = 200.0f;
    distanceToNearestObstacle = MAX_CHECK_DISTANCE;
    
    // Check in 8 directions (N, NE, E, SE, S, SW, W, NW)
    for (int angle = 0; angle < 360; angle += 45) {
        float radian = angle * 3.14159f / 180.0f;
        float dx = std::cos(radian);
        float dy = std::sin(radian);
        
        // Check points along this direction
        for (float dist = 10.0f; dist <= MAX_CHECK_DISTANCE; dist += 10.0f) {
            sf::Vector2f checkPoint(position.x + dx * dist, position.y + dy * dist);
            
            if (environment.isObstacle(checkPoint)) {
                // Found an obstacle, update nearest distance if closer
                if (dist < distanceToNearestObstacle) {
                    distanceToNearestObstacle = dist;
                }
                break;
            }
        }
    }
}

int EnvironmentState::determineCurrentRoom()
{
    // In a real implementation, this would determine which room the character is in
    // based on the environment's room definitions
    
    // Simplified implementation - divide the space into four quadrants
    int roomId = 0;
    if (position.x > 320) { // Right half
        if (position.y > 240) { // Bottom-right
            roomId = 3;
        } else { // Top-right
            roomId = 1;
        }
    } else { // Left half
        if (position.y > 240) { // Bottom-left
            roomId = 2;
        } else { // Top-left
            roomId = 0;
        }
    }
    
    return roomId;
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
    // In a real implementation, this would check if there are obstacles blocking the path
    // Simplified implementation
    return pathBlocked;
}

bool EnvironmentState::canSeeTarget(const sf::Vector2f &target) const
{
    return hasLineOfSightTo(target);
}

bool EnvironmentState::isNearWall() const
{
    // Check if close to a wall/boundary
    return distanceToNearestObstacle < 30.0f;
}

bool EnvironmentState::isInCenterOfRoom() const
{
    // Simple check to see if we're near the center of the current room
    // In a real implementation, this would use the actual room dimensions
    
    // Calculate approximate room centers based on quadrants
    sf::Vector2f roomCenters[4] = {
        {160, 120},  // Top-left room
        {480, 120},  // Top-right room
        {160, 360},  // Bottom-left room
        {480, 360}   // Bottom-right room
    };
    
    sf::Vector2f roomCenter = roomCenters[currentRoom];
    float distanceToCenter = getDistanceToTarget(roomCenter);
    
    return distanceToCenter < 50.0f;
}

bool EnvironmentState::hasLineOfSightTo(const sf::Vector2f &target) const
{
    // Check if we have a clear line of sight to the target
    return environment.hasLineOfSight(position, target);
}

bool EnvironmentState::isMovingTowards(const sf::Vector2f &target) const
{
    if (speed < 5.0f) {
        return false; // Not really moving
    }
    
    // Calculate direction to target
    sf::Vector2f dirToTarget = target - position;
    float targetDistance = std::sqrt(dirToTarget.x * dirToTarget.x + dirToTarget.y * dirToTarget.y);
    
    if (targetDistance < 0.1f) {
        return true; // Already at target
    }
    
    // Normalize
    dirToTarget /= targetDistance;
    
    // Normalize velocity
    sf::Vector2f normVelocity = velocity;
    normVelocity /= speed;
    
    // Dot product gives cosine of angle between vectors
    float dotProduct = dirToTarget.x * normVelocity.x + dirToTarget.y * normVelocity.y;
    
    // If dot product > 0.7, angle is less than ~45 degrees (we're moving towards target)
    return dotProduct > 0.7f;
}

bool EnvironmentState::isIdleForTooLong(float threshold) const
{
    return isIdle && idleTimer.getElapsedTime().asSeconds() >= threshold;
}

bool EnvironmentState::shouldChangeTarget() const
{
    // Logic to determine if we should change targets
    // For example, if we've been in the same state too long or are stuck
    return hasBeenInCurrentState(5.0f) || (isIdle && isIdleForTooLong(2.0f));
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

std::shared_ptr<DecisionNode> DecisionTree::createTargetSubtree(const sf::Vector2f &target)
{
    // Create a subtree for pursuing a specific target
    
    // Define conditions for this target
    auto isTargetVisible = [this, target]() {
        return environmentState.canSeeTarget(target);
    };
    
    auto isNearTarget = [this, target]() {
        return environmentState.getDistanceToTarget(target) < 50.0f;
    };
    
    auto isMovingToTarget = [this, target]() {
        return environmentState.isMovingTowards(target);
    };
    
    auto shouldSeekNewTarget = [this]() {
        return environmentState.shouldChangeTarget();
    };
    
    // Create actions
    auto pathfindAction = std::make_shared<ActionNode>("PathfindToPlayer");
    auto arriveAction = std::make_shared<ActionNode>("Arrive");
    auto wanderAction = std::make_shared<ActionNode>("Wander");
    
    // Build subtree
    auto approachDecision = std::make_shared<DecisionBranch>(
        isNearTarget,
        arriveAction,  // If near target, use arrive behavior
        pathfindAction, // Otherwise pathfind to it
        "Is near target?"
    );
    
    auto visibilityDecision = std::make_shared<DecisionBranch>(
        isTargetVisible,
        approachDecision, // If visible, approach it
        wanderAction,     // Otherwise wander
        "Is target visible?"
    );
    
    auto targetPersistenceDecision = std::make_shared<DecisionBranch>(
        shouldSeekNewTarget,
        wanderAction,        // If should change target, wander
        visibilityDecision,  // Otherwise continue with current target
        "Should change target?"
    );
    
    return targetPersistenceDecision;
}

void DecisionTree::buildComplexTree(const std::vector<sf::Vector2f> &targets)
{
    // Create more sophisticated decision tree with various behaviors
    
    // Ensure we have some targets
    if (targets.empty()) {
        rootNode = std::make_shared<ActionNode>("Idle");
        return;
    }
    
    // Random target selection for variety
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, targets.size() - 1);
    
    // Define common conditions and actions
    auto isNearObstacle = [this]() { return environmentState.isNearObstacle(40.0f); };
    auto isMovingFast = [this]() { return environmentState.isMovingFast(150.0f); };
    auto isNearWall = [this]() { return environmentState.isNearWall(); };
    auto shouldDance = [this]() { return (rand() % 100) < 5; }; // 5% chance to dance
    auto isIdleTooLong = [this]() { return environmentState.isIdleForTooLong(3.0f); };
    
    // Create action nodes
    auto fleeAction = std::make_shared<ActionNode>("Flee");
    auto wanderAction = std::make_shared<ActionNode>("Wander");
    auto pathfindAction = std::make_shared<ActionNode>("PathfindToPlayer");
    auto arriveAction = std::make_shared<ActionNode>("Arrive");
    auto danceAction = std::make_shared<ActionNode>("Dance");
    auto idleAction = std::make_shared<ActionNode>("Idle");
    
    // Create target-specific subtrees
    std::vector<std::shared_ptr<DecisionNode>> targetSubtrees;
    for (const auto &target : targets) {
        environmentState.setTarget(target);
        targetSubtrees.push_back(createTargetSubtree(target));
    }
    
    // Create a random decision node for target selection
    auto targetSelectionNode = std::make_shared<RandomDecisionNode>("Target Selection");
    for (const auto &subtree : targetSubtrees) {
        targetSelectionNode->addChild(subtree);
    }
    
    // Special behavior decision: Dance occasionally
    auto specialBehaviorNode = std::make_shared<DecisionBranch>(
        shouldDance,
        danceAction,
        targetSelectionNode,
        "Should perform special behavior?"
    );
    
    // Movement safety decision: Flee from obstacles when moving fast
    auto safetyNode = std::make_shared<DecisionBranch>(
        isNearObstacle,
        fleeAction, // If near obstacle, flee
        specialBehaviorNode, // Otherwise continue with normal behaviors
        "Is near obstacle?"
    );
    
    // Main activity decision: Safety vs. Idle
    auto activityNode = std::make_shared<DecisionBranch>(
        isMovingFast,
        safetyNode, // If moving fast, check safety
        std::make_shared<DecisionBranch>(
            isIdleTooLong,
            wanderAction, // If idle too long, wander
            targetSelectionNode, // Otherwise continue with normal behaviors
            "Idle too long?"
        ),
        "Is moving fast?"
    );
    
    // Set the root node
    setRootNode(activityNode);
    
    // Debug output
    //std::cout << "Decision Tree Structure:\n" << printTree() << std::endl;
}

std::string DecisionTree::printTree(std::shared_ptr<DecisionNode> node, int depth) const
{
    if (!node) {
        if (depth == 0) {
            node = rootNode;
        } else {
            return "";
        }
    }
    
    if (!node) {
        return "Empty tree";
    }
    
    std::stringstream ss;
    std::string indent(depth * 2, ' ');
    ss << indent << node->getName() << std::endl;
    
    // For a more complete implementation, you'd need RTTI or a visitor pattern
    // to traverse the tree. This is a simplified version.
    
    return ss.str();
}