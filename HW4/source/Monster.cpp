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

    // Check if monster has caught player
    bool caughtPlayer = hasCaughtPlayer();

    // Determine action based on control type
    if (controlType == ControlType::BEHAVIOR_TREE)
    {
        if (behaviorTree)
        {
            // Tick the behavior tree
            BehaviorStatus status = behaviorTree->tick();

            // Note: The behavior tree actions should directly call monster methods
            // The monster's current action is set by those methods
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

    // Drop a breadcrumb for visualization
    breadcrumbCounter++;
    if (breadcrumbCounter >= BREADCRUMB_INTERVAL)
    {
        breadcrumbCounter = 0;
        dropBreadcrumb();
    }

    // Update sprite
    updateSprite();

    return caughtPlayer;
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
    bool canSeePlayer = true; // Simplified - would use environment.hasLineOfSight() in full implementation

    // Determine if obstacles are nearby
    bool isNearObstacle = false; // Simplified - would check environment in full implementation

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
    // Simple wander behavior - add a random steering force
    float wanderRadius = 100.0f;
    float wanderDistance = 50.0f;
    float wanderJitter = 10.0f;

    // Maintain a static random target position on the wander circle
    static sf::Vector2f wanderTarget = {0, 0};

    // Jitter the target
    wanderTarget.x += (rand() % (int)(wanderJitter * 2) - wanderJitter) * deltaTime;
    wanderTarget.y += (rand() % (int)(wanderJitter * 2) - wanderJitter) * deltaTime;

    // Normalize the target
    float length = std::sqrt(wanderTarget.x * wanderTarget.x + wanderTarget.y * wanderTarget.y);
    if (length > 0)
    {
        wanderTarget = wanderTarget * (wanderRadius / length);
    }
    else
    {
        wanderTarget = {wanderRadius, 0};
    }

    // Calculate the target position in world space
    sf::Vector2f targetWorld = monsterKinematic.position;

    // Add velocity direction multiplied by distance
    if (std::abs(monsterKinematic.velocity.x) + std::abs(monsterKinematic.velocity.y) > 0.1f)
    {
        sf::Vector2f velNorm = monsterKinematic.velocity;
        float velLength = std::sqrt(velNorm.x * velNorm.x + velNorm.y * velNorm.y);
        velNorm = velNorm * (1.0f / velLength);
        targetWorld += velNorm * wanderDistance;
    }
    else
    {
        // If not moving, use orientation instead
        float radians = monsterKinematic.orientation * 3.14159f / 180.0f;
        targetWorld += sf::Vector2f(std::cos(radians), std::sin(radians)) * wanderDistance;
    }

    // Add the target offset
    targetWorld += wanderTarget;

    // Create a temporary target kinematic
    Kinematic targetKinematic;
    targetKinematic.position = targetWorld;

    // Calculate steering using arrive behavior
    SteeringData steering = arriveBehavior.calculateAcceleration(monsterKinematic, targetKinematic);

    // Apply steering
    monsterKinematic.velocity += steering.linear * deltaTime;
    monsterKinematic.rotation += steering.angular * deltaTime;

    // Update position
    monsterKinematic.update(deltaTime);
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

    // Update position
    monsterKinematic.update(deltaTime);

    // Check if we've reached the waypoint
    if (distance < 15.0f)
    {
        currentWaypointIndex++;
    }
}

void Monster::doDance(float deltaTime)
{
    // Ensure we have a dance path
    if (dancePath.empty())
    {
        return;
    }

    // Initialize dance if needed
    if (!isDancing)
    {
        isDancing = true;
        danceTimer = 0;
        currentWaypointIndex = 0;
    }

    // Get current dance point
    sf::Vector2f targetPos = dancePath[currentWaypointIndex % dancePath.size()];

    // Create target kinematic
    Kinematic targetKinematic;
    targetKinematic.position = targetPos;

    // Calculate direction to dance point
    sf::Vector2f direction = targetPos - monsterKinematic.position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // Calculate desired orientation
    if (distance > 0.1f)
    {
        float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159f;
        targetKinematic.orientation = angle;
    }

    // Apply arrive behavior to move to dance point
    SteeringData steering = arriveBehavior.calculateAcceleration(monsterKinematic, targetKinematic);

    // Apply align behavior to face the right direction
    SteeringData alignSteering = alignBehavior.calculateAcceleration(monsterKinematic, targetKinematic);
    steering.angular = alignSteering.angular;

    // Apply steering
    monsterKinematic.velocity += steering.linear * deltaTime;
    monsterKinematic.rotation += steering.angular * deltaTime;

    // Update position
    monsterKinematic.update(deltaTime);

    // Update dance timer
    danceTimer += deltaTime;

    // Check if we've reached the dance point
    if (distance < 10.0f)
    {
        currentWaypointIndex++;
    }

    // End dance after a certain time
    if (danceTimer > 5.0f)
    {
        isDancing = false;
    }
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