/**
 * @file Monster.cpp
 * @brief Implementation of the Monster class.
 *
 * Author: Miles Hollifield
 * Date: 4/7/2025
 */

#include "headers/Monster.h"
#include "headers/BehaviorTree.h"
#include "headers/DecisionTree.h"
#include "headers/Dijkstra.h"
#include <cmath>
#include <iostream>
#include <fstream>

Monster::Monster(sf::Vector2f startPosition, sf::Texture &texture, Environment &environment, Graph &graph, sf::Color color)
    : monsterKinematic(startPosition),
      startPosition(startPosition),
      environment(environment),
      navigationGraph(graph),
      playerKinematic(nullptr),
      currentWaypointIndex(0),
      controlType(ControlType::BEHAVIOR_TREE),
      currentDeltaTime(0.0f),
      timeInCurrentAction(0),
      catchDistance(30.0f),
      isDancing(false),
      danceTimer(0),
      breadcrumbCounter(0),
      // Initialize behaviors with appropriate parameters
      arriveBehavior(150.0f, 120.0f, 15.0f, 80.0f, 0.1f),
      alignBehavior(20.0f, 180.0f, 1.0f, 30.0f, 0.1f)
{
    // Initialize sprite
    sprite.setTexture(texture);
    sprite.setScale(0.05f, 0.05f); // Reduced scale to match player size
    sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
    sprite.setPosition(startPosition);

    // Set the sprite color to distinguish it from the player
    sprite.setColor(color);

    // Set up dance path (a simple circle)
    for (int i = 0; i < 12; i++)
    {
        float angle = i * 2 * 3.14159f / 12;
        float radius = 30.0f; // Smaller dance radius to match smaller scale
        sf::Vector2f point = startPosition + sf::Vector2f(std::cos(angle) * radius, std::sin(angle) * radius);
        dancePath.push_back(point);
    }
}

void Monster::setControlType(ControlType type)
{
    controlType = type;
    currentAction = "Idle"; // Reset action
    reset();                // Reset position and state
}

void Monster::setPlayerKinematic(const Kinematic &playerKinematic)
{
    this->playerKinematic = &playerKinematic;
}

void Monster::setBehaviorTree(std::shared_ptr<BehaviorTree> tree)
{
    behaviorTree = tree;
}

void Monster::setDecisionTree(std::shared_ptr<DecisionTree> tree)
{
    decisionTree = tree;
}

void Monster::reset()
{
    // Reset position and velocity
    monsterKinematic.position = startPosition;
    monsterKinematic.velocity = {0, 0};
    monsterKinematic.orientation = 0;
    monsterKinematic.rotation = 0;

    // Reset path following
    currentPath.clear();
    currentWaypointIndex = 0;

    // Reset state
    isDancing = false;
    danceTimer = 0;
    timeInCurrentAction = 0;

    // Reset behaviors
    if (behaviorTree)
    {
        behaviorTree->reset();
    }

    // Clear breadcrumbs
    breadcrumbs.clear();

    // Update sprite
    updateSprite();
}

bool Monster::update(float deltaTime)
{
    // Update time in current action
    timeInCurrentAction += deltaTime;

    // Store the current deltaTime for use by behavior tree actions
    setDeltaTime(deltaTime);

    // Check if monster has caught player
    bool caughtPlayer = hasCaughtPlayer();

    // Determine action based on control type
    if (controlType == ControlType::BEHAVIOR_TREE)
    {
        if (behaviorTree)
        {
            // Tick the behavior tree
            BehaviorStatus status = behaviorTree->tick();

            // If no action was set by the behavior tree, default to idle
            if (currentAction.empty())
            {
                currentAction = "Idle";
            }
        }
    }
    else if (controlType == ControlType::DECISION_TREE)
    {
        if (decisionTree)
        {
            // Get decision from decision tree
            std::string action = decisionTree->makeDecision();
            executeAction(action, deltaTime);
        }
    }

    // Execute the current action
    executeAction(currentAction, deltaTime);

    // Drop breadcrumb and update sprite
    dropBreadcrumb();
    updateSprite();

    // Return whether we've caught the player
    return hasCaughtPlayer();
}

void Monster::draw(sf::RenderWindow &window)
{
    // Draw breadcrumbs
    for (const auto &crumb : breadcrumbs)
    {
        window.draw(crumb);
    }

    // Draw current path if any
    if (!currentPath.empty())
    {
        sf::VertexArray lines(sf::LinesStrip, currentPath.size());
        for (size_t i = 0; i < currentPath.size(); i++)
        {
            lines[i].position = currentPath[i];
            lines[i].color = sf::Color(255, 100, 100, 150); // Semi-transparent red
        }
        window.draw(lines);
    }

    // Draw sprite
    window.draw(sprite);
}

sf::Vector2f Monster::getPosition() const
{
    return monsterKinematic.position;
}

const Kinematic &Monster::getKinematic() const
{
    return monsterKinematic;
}

bool Monster::hasCaughtPlayer() const
{
    if (!playerKinematic)
    {
        return false;
    }

    float dx = playerKinematic->position.x - monsterKinematic.position.x;
    float dy = playerKinematic->position.y - monsterKinematic.position.y;
    float distanceSquared = dx * dx + dy * dy;

    return distanceSquared < (catchDistance * catchDistance);
}

void Monster::recordStateAction(std::ofstream &outputFile)
{
    if (!outputFile.is_open() || !playerKinematic)
    {
        return;
    }

    // Calculate state variables
    float distanceToPlayer = std::sqrt(
        std::pow(playerKinematic->position.x - monsterKinematic.position.x, 2) +
        std::pow(playerKinematic->position.y - monsterKinematic.position.y, 2));

    float relativeOrientation = playerKinematic->orientation - monsterKinematic.orientation;
    while (relativeOrientation > 180)
        relativeOrientation -= 360;
    while (relativeOrientation < -180)
        relativeOrientation += 360;

    float speed = std::sqrt(
        monsterKinematic.velocity.x * monsterKinematic.velocity.x +
        monsterKinematic.velocity.y * monsterKinematic.velocity.y);

    // Determine if player is in line of sight
    bool canSeePlayer = environment.hasLineOfSight(monsterKinematic.position, playerKinematic->position);

    // Determine if obstacles are nearby
    bool isNearObstacle = false;
    const float CHECK_DISTANCE = 50.0f;

    // Check in 8 directions (N, NE, E, SE, S, SW, W, NW)
    for (int angle = 0; angle < 360; angle += 45)
    {
        float radian = angle * 3.14159f / 180.0f;
        float dx = std::cos(radian);
        float dy = std::sin(radian);

        sf::Vector2f checkPoint(monsterKinematic.position.x + dx * CHECK_DISTANCE,
                                monsterKinematic.position.y + dy * CHECK_DISTANCE);

        if (environment.isObstacle(checkPoint))
        {
            isNearObstacle = true;
            break;
        }
    }

    // Count how many paths have been generated
    int pathCount = currentPath.size();

    // Write state-action record to file
    outputFile << distanceToPlayer << ","
               << relativeOrientation << ","
               << speed << ","
               << (canSeePlayer ? "1" : "0") << ","
               << (isNearObstacle ? "1" : "0") << ","
               << pathCount << ","
               << timeInCurrentAction << ","
               << currentAction << std::endl;
}

void Monster::executeAction(const std::string &action, float deltaTime)
{
    // Check if this is a new action
    if (action != currentAction)
    {
        currentAction = action;
        timeInCurrentAction = 0;
    }

    // Execute the specified action
    if (action == "PathfindToPlayer")
    {
        pathfindToPlayer();
        followPath(deltaTime);
    }
    else if (action == "Wander")
    {
        wander(deltaTime);
    }
    else if (action == "FollowPath")
    {
        followPath(deltaTime);
    }
    else if (action == "Dance")
    {
        doDance(deltaTime);
    }
    else if (action == "Flee")
    {
        flee(deltaTime);
    }
    else if (action == "Idle")
    {
        // Do nothing
    }
    else
    {
        std::cerr << "Unknown action: " << action << std::endl;
    }
}

void Monster::pathfindToPlayer()
{
    if (!playerKinematic)
    {
        return;
    }

    // Convert positions to graph vertices
    int monsterVertex = environment.pointToVertex(monsterKinematic.position);
    int playerVertex = environment.pointToVertex(playerKinematic->position);

    // Create pathfinder
    Dijkstra pathfinder;

    // Find path
    std::vector<int> path = pathfinder.findPath(navigationGraph, monsterVertex, playerVertex);

    // Convert path to waypoints
    currentPath.clear();
    for (int vertex : path)
    {
        currentPath.push_back(navigationGraph.getVertexPosition(vertex));
    }

    // Reset waypoint index
    currentWaypointIndex = 0;
}

void Monster::wander(float deltaTime)
{
    // Calculate wander circle center ahead of the monster
    float wanderCircleDistance = 50.0f;
    float wanderCircleRadius = 30.0f;

    // Get normalized velocity direction (or use orientation if velocity is near zero)
    sf::Vector2f direction;
    if (std::abs(monsterKinematic.velocity.x) + std::abs(monsterKinematic.velocity.y) > 0.1f)
    {
        float speed = std::sqrt(monsterKinematic.velocity.x * monsterKinematic.velocity.x +
                                monsterKinematic.velocity.y * monsterKinematic.velocity.y);
        direction = monsterKinematic.velocity / speed;
    }
    else
    {
        float angle = monsterKinematic.orientation * 3.14159f / 180.0f;
        direction = sf::Vector2f(std::cos(angle), std::sin(angle));
    }

    // Calculate wander circle center
    sf::Vector2f circleCenter = monsterKinematic.position + direction * wanderCircleDistance;

    // Update wander angle with some randomness
    static float wanderAngle = 0;
    wanderAngle += ((rand() % 100) / 100.0f - 0.5f) * 30.0f; // Random angle change

    // Calculate displacement force
    sf::Vector2f displacement(std::cos(wanderAngle * 3.14159f / 180.0f),
                              std::sin(wanderAngle * 3.14159f / 180.0f));
    displacement *= wanderCircleRadius;

    // Calculate target and apply steering
    sf::Vector2f wanderForce = circleCenter + displacement - monsterKinematic.position;

    // Normalize and scale force
    float length = std::sqrt(wanderForce.x * wanderForce.x + wanderForce.y * wanderForce.y);
    if (length > 0)
    {
        wanderForce /= length;
        wanderForce *= 100.0f; // Scale to desired acceleration
    }

    // Apply force to velocity
    monsterKinematic.velocity += wanderForce * deltaTime;

    // Limit speed
    float currentSpeed = std::sqrt(monsterKinematic.velocity.x * monsterKinematic.velocity.x +
                                   monsterKinematic.velocity.y * monsterKinematic.velocity.y);
    if (currentSpeed > 50.0f)
    {
        monsterKinematic.velocity *= (50.0f / currentSpeed);
    }

    // Calculate proposed position with collision check
    sf::Vector2f proposedPosition = monsterKinematic.position + monsterKinematic.velocity * deltaTime;

    if (!checkCollision(proposedPosition))
    {
        // Safe to move
        monsterKinematic.position = proposedPosition;
    }
    else
    {
        // Hit obstacle, find valid movement or change direction
        sf::Vector2f validPosition = findValidMovement(monsterKinematic.position, proposedPosition);

        if (validPosition != monsterKinematic.position)
        {
            // Move to valid position
            monsterKinematic.position = validPosition;
        }
        else
        {
            // Completely stuck, change direction dramatically
            float randomAngle = (rand() % 360) * 3.14159f / 180.0f;
            monsterKinematic.velocity = sf::Vector2f(std::cos(randomAngle), std::sin(randomAngle)) * 50.0f;

            // Reset wander angle to prevent getting stuck in a pattern
            wanderAngle = randomAngle * 180.0f / 3.14159f;
        }
    }

    // Update orientation
    if (currentSpeed > 0.1f)
    {
        monsterKinematic.orientation = std::atan2(monsterKinematic.velocity.y,
                                                  monsterKinematic.velocity.x) *
                                       180.0f / 3.14159f;
    }
}

void Monster::followPath(float deltaTime)
{
    // Check if we have a path
    if (currentPath.empty() || currentWaypointIndex >= currentPath.size())
    {
        return;
    }

    // Get current waypoint
    sf::Vector2f targetPos = currentPath[currentWaypointIndex];

    // Create target kinematic
    Kinematic targetKinematic;
    targetKinematic.position = targetPos;

    // Calculate direction to waypoint
    sf::Vector2f direction = targetPos - monsterKinematic.position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // Calculate desired orientation
    if (distance > 0.1f)
    {
        float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159f;
        targetKinematic.orientation = angle;
    }

    // Apply arrive behavior to move to waypoint
    SteeringData steering = arriveBehavior.calculateAcceleration(monsterKinematic, targetKinematic);

    // Apply align behavior to face the right direction
    SteeringData alignSteering = alignBehavior.calculateAcceleration(monsterKinematic, targetKinematic);
    steering.angular = alignSteering.angular;

    // Apply steering
    monsterKinematic.velocity += steering.linear * deltaTime;
    monsterKinematic.rotation += steering.angular * deltaTime;

    // Calculate proposed position with collision check
    sf::Vector2f proposedPosition = monsterKinematic.position + monsterKinematic.velocity * deltaTime;

    if (!checkCollision(proposedPosition))
    {
        // Safe to move
        monsterKinematic.update(deltaTime);
    }
    else
    {
        // Hit obstacle, stop and recalculate path
        monsterKinematic.velocity = {0, 0};
        // Recalculate path if target is player
        if (playerKinematic)
        {
            pathfindToPlayer();
        }
    }

    // Check if we've reached the waypoint
    if (distance < 15.0f)
    {
        currentWaypointIndex++;
    }
}

void Monster::doDance(float deltaTime) {
    // Initialize dance if needed
    if (!isDancing) {
        isDancing = true;
        danceTimer = 0;
        
        // Set target rotation speed (degrees per second)
        static const float ROTATION_SPEED = 360.0f; // One full rotation per second
        monsterKinematic.rotation = ROTATION_SPEED;
        
        // Stop movement during dance
        monsterKinematic.velocity = {0, 0};
    }
    
    // Update dance timer
    danceTimer += deltaTime;
    
    // Rotate in place
    monsterKinematic.orientation += monsterKinematic.rotation * deltaTime;
    
    // Keep orientation in range [0, 360)
    while (monsterKinematic.orientation >= 360.0f) {
        monsterKinematic.orientation -= 360.0f;
    }
    
    // End dance after two rotations (2 seconds at 360 degrees/second)
    if (danceTimer > 2.0f) {
        isDancing = false;
        monsterKinematic.rotation = 0;
    }
}

void Monster::flee(float deltaTime)
{
    sf::Vector2f fleeDirection = {0, 0};
    float nearestObstacleDistance = 1000.0f;

    // Check in 8 directions to find nearest obstacle
    for (int angle = 0; angle < 360; angle += 45)
    {
        float radian = angle * 3.14159f / 180.0f;
        float dx = std::cos(radian);
        float dy = std::sin(radian);

        // Check points along this direction
        for (float dist = 10.0f; dist <= 100.0f; dist += 10.0f)
        {
            sf::Vector2f checkPoint(monsterKinematic.position.x + dx * dist,
                                    monsterKinematic.position.y + dy * dist);

            if (environment.isObstacle(checkPoint))
            {
                if (dist < nearestObstacleDistance)
                {
                    nearestObstacleDistance = dist;
                    // Flee in opposite direction
                    fleeDirection = sf::Vector2f(-dx, -dy);
                }
                break;
            }
        }
    }

    // If no obstacles found, flee away from last known position
    if (fleeDirection.x == 0 && fleeDirection.y == 0)
    {
        fleeDirection = monsterKinematic.position - sf::Vector2f(monsterKinematic.position.x + monsterKinematic.velocity.x,
                                                                 monsterKinematic.position.y + monsterKinematic.velocity.y);

        float length = std::sqrt(fleeDirection.x * fleeDirection.x + fleeDirection.y * fleeDirection.y);
        if (length > 0)
        {
            fleeDirection /= length;
        }
        else
        {
            // Random direction
            float angle = (rand() % 360) * 3.14159f / 180.0f;
            fleeDirection = sf::Vector2f(std::cos(angle), std::sin(angle));
        }
    }

    // Set velocity to flee
    float fleeSpeed = 200.0f;
    monsterKinematic.velocity = fleeDirection * fleeSpeed;

    // Update orientation to match flee direction
    monsterKinematic.orientation = std::atan2(fleeDirection.y, fleeDirection.x) * 180.0f / 3.14159f;

    // Check for collision
    sf::Vector2f proposedPosition = monsterKinematic.position + monsterKinematic.velocity * deltaTime;

    if (!checkCollision(proposedPosition))
    {
        // Safe to move
        monsterKinematic.update(deltaTime);
    }
    else
    {
        // Try to find a valid movement direction
        sf::Vector2f validPosition = findValidMovement(monsterKinematic.position, proposedPosition);
        monsterKinematic.position = validPosition;
    }
}

bool Monster::checkCollision(sf::Vector2f proposedPosition) const
{
    return environment.isObstacle(proposedPosition);
}

sf::Vector2f Monster::findValidMovement(sf::Vector2f currentPos, sf::Vector2f proposedPos) const
{
    // Calculate movement vector
    sf::Vector2f movementVector = proposedPos - currentPos;
    float movementLength = std::sqrt(movementVector.x * movementVector.x + movementVector.y * movementVector.y);

    if (movementLength < 0.01f)
    {
        return currentPos; // No movement
    }

    // Try moving in just X direction
    sf::Vector2f xOnlyPos(proposedPos.x, currentPos.y);
    if (!environment.isObstacle(xOnlyPos))
    {
        return xOnlyPos;
    }

    // Try moving in just Y direction
    sf::Vector2f yOnlyPos(currentPos.x, proposedPos.y);
    if (!environment.isObstacle(yOnlyPos))
    {
        return yOnlyPos;
    }

    // Try various fractions of the movement
    for (float fraction = 0.75f; fraction >= 0.1f; fraction -= 0.15f)
    {
        sf::Vector2f fractionPos = currentPos + movementVector * fraction;
        if (!environment.isObstacle(fractionPos))
        {
            return fractionPos;
        }
    }

    // No valid position found, stay in place
    return currentPos;
}

void Monster::updateSprite()
{
    sprite.setPosition(monsterKinematic.position);
    sprite.setRotation(monsterKinematic.orientation);
}

void Monster::dropBreadcrumb()
{
    sf::CircleShape crumb(3.0f);
    crumb.setFillColor(sf::Color(255, 0, 0, 150)); // Semi-transparent red
    crumb.setPosition(monsterKinematic.position - sf::Vector2f(3.0f, 3.0f));
    breadcrumbs.push_back(crumb);

    // Limit the number of breadcrumbs
    if (breadcrumbs.size() > MAX_BREADCRUMBS)
    {
        breadcrumbs.pop_front();
    }
}