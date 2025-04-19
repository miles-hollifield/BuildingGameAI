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

            // Note: The behavior tree actions already call executeAction()
            // No need to call executeAction() again for this control type

            // If no action was set by the behavior tree, default to idle
            if (currentAction.empty())
            {
                currentAction = "Idle";
                executeAction(currentAction, deltaTime); // Only execute if setting to idle
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

void Monster::pathfindToPlayer() {
    if (!playerKinematic) {
        std::cout << "PATHFIND: No player kinematic set!" << std::endl;
        return;
    }

    std::cout << "PATHFIND: Finding path to player at " 
              << playerKinematic->position.x << "," << playerKinematic->position.y << std::endl;

    // Convert positions to graph vertices
    int monsterVertex = environment.pointToVertex(monsterKinematic.position);
    int playerVertex = environment.pointToVertex(playerKinematic->position);

    // Create pathfinder
    Dijkstra pathfinder;

    // Find path
    std::vector<int> path = pathfinder.findPath(navigationGraph, monsterVertex, playerVertex);

    if (path.empty()) {
        std::cout << "PATHFIND: No path found to player!" << std::endl;
        return;
    }

    std::cout << "PATHFIND: Found path with " << path.size() << " waypoints" << std::endl;

    // Convert path to waypoints
    currentPath.clear();
    for (int vertex : path) {
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

void Monster::followPath(float deltaTime) {
    // Check if we have a path
    if (currentPath.empty() || currentWaypointIndex >= currentPath.size()) {
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

    if (distance > 0.1f) {
        // Normalize direction vector
        direction /= distance;
        // Set orientation directly toward the current waypoint
        float desiredOrientation = std::atan2(direction.y, direction.x) * (180.0f / 3.14159265f);
        targetKinematic.orientation = desiredOrientation;
    }

    // Apply arrive behavior to move to waypoint
    SteeringData steering = arriveBehavior.calculateAcceleration(monsterKinematic, targetKinematic);

    // Apply align behavior to face the right direction
    SteeringData alignSteering = alignBehavior.calculateAcceleration(monsterKinematic, targetKinematic);
    steering.angular = alignSteering.angular;

    // Apply steering
    monsterKinematic.velocity += steering.linear * deltaTime;
    monsterKinematic.rotation += steering.angular * deltaTime;

    // Limit velocity to max speed
    float currentSpeed = std::sqrt(monsterKinematic.velocity.x * monsterKinematic.velocity.x +
                                  monsterKinematic.velocity.y * monsterKinematic.velocity.y);
    const float MAX_SPEED = 150.0f;
    if (currentSpeed > MAX_SPEED) {
        monsterKinematic.velocity *= (MAX_SPEED / currentSpeed);
    }

    // Update position and orientation
    monsterKinematic.update(deltaTime);

    // Check if we've reached the waypoint
    const float WAYPOINT_THRESHOLD = 15.0f;
    if (distance < WAYPOINT_THRESHOLD) {
        std::cout << "Monster reached waypoint " << currentWaypointIndex + 1 << "/" << currentPath.size() << std::endl;
        currentWaypointIndex++;
        
        // If we've reached the end of the path, reset
        if (currentWaypointIndex >= currentPath.size()) {
            currentPath.clear();
            currentWaypointIndex = 0;
        }
    }
}

void Monster::doDance(float deltaTime)
{
    // Initialize dance if needed
    if (!isDancing)
    {
        std::cout << "MONSTER: Starting dance" << std::endl;
        isDancing = true;
        danceTimer = 0;
        dancePhase = 0;

        // Stop movement during dance
        monsterKinematic.velocity = {0, 0};

        // Initial orientation - North (270 degrees)
        monsterKinematic.orientation = 270.0f;
    }

    // Update dance timer
    danceTimer += deltaTime;

    // Cardinal directions dance:
    // Phase 0 (0-0.5s): North (270°)
    // Phase 1 (0.5-1.0s): East (0°)
    // Phase 2 (1.0-1.5s): South (90°)
    // Phase 3 (1.5-2.0s): West (180°)

    const float PHASE_DURATION = 0.5f;
    float phaseTimes[4] = {0.0f, 0.5f, 1.0f, 1.5f};
    float orientations[4] = {270.0f, 0.0f, 90.0f, 180.0f};

    // Set the current phase based on timer
    for (int i = 0; i < 4; i++)
    {
        if (danceTimer >= phaseTimes[i] &&
            (i == 3 || danceTimer < phaseTimes[i + 1]))
        {
            if (dancePhase != i)
            {
                std::cout << "MONSTER: Dance phase " << i << ", orientation = " << orientations[i] << std::endl;
                dancePhase = i;
            }
            // Set orientation based on current phase
            monsterKinematic.orientation = orientations[dancePhase];
            break;
        }
    }

    // End dance after all 4 directions (2 seconds total)
    if (danceTimer >= 2.0f)
    {
        std::cout << "MONSTER: Dance completed, time = " << danceTimer << std::endl;
        isDancing = false;

        // Resume movement with a small random velocity
        float randomAngle = (rand() % 360) * 3.14159f / 180.0f;
        monsterKinematic.velocity = sf::Vector2f(std::cos(randomAngle), std::sin(randomAngle)) * 20.0f;
    }
}

bool Monster::hasLineOfSightTo(const sf::Vector2f &target) const
{
    return environment.hasLineOfSight(monsterKinematic.position, target);
}

void Monster::flee(float deltaTime)
{
    std::cout << "FLEE: Starting flee behavior" << std::endl;

    sf::Vector2f fleeDirection = {0, 0};
    float nearestObstacleDistance = 1000.0f;

    // Current movement direction
    sf::Vector2f moveDir(0, 0);
    float speed = std::sqrt(monsterKinematic.velocity.x * monsterKinematic.velocity.x +
                            monsterKinematic.velocity.y * monsterKinematic.velocity.y);
    if (speed > 0.1f)
    {
        moveDir = monsterKinematic.velocity / speed;
    }

    // Check in 8 directions to find obstacles
    std::vector<int> obstacleDirections;
    for (int angle = 0; angle < 360; angle += 45)
    {
        float radian = angle * 3.14159f / 180.0f;
        float dx = std::cos(radian);
        float dy = std::sin(radian);
        sf::Vector2f rayDir(dx, dy);

        // Check points along this direction at closer distances
        for (float dist = 5.0f; dist <= 30.0f; dist += 5.0f)
        {
            sf::Vector2f checkPoint(monsterKinematic.position.x + dx * dist,
                                    monsterKinematic.position.y + dy * dist);

            if (environment.isObstacle(checkPoint))
            {
                // Mark this direction as having an obstacle
                obstacleDirections.push_back(angle);

                if (dist < nearestObstacleDistance)
                {
                    nearestObstacleDistance = dist;
                    std::cout << "FLEE: Found obstacle at distance " << dist
                              << " in direction " << angle << " degrees" << std::endl;
                }
                break;
            }
        }
    }

    // Determine best flee direction based on obstacle locations
    if (!obstacleDirections.empty())
    {
        // Calculate the average obstacle direction
        float avgX = 0, avgY = 0;
        for (int angle : obstacleDirections)
        {
            float radian = angle * 3.14159f / 180.0f;
            avgX += std::cos(radian);
            avgY += std::sin(radian);
        }
        avgX /= obstacleDirections.size();
        avgY /= obstacleDirections.size();

        // Flee in opposite direction of average obstacle
        fleeDirection = sf::Vector2f(-avgX, -avgY);
        float length = std::sqrt(fleeDirection.x * fleeDirection.x + fleeDirection.y * fleeDirection.y);
        if (length > 0)
        {
            fleeDirection /= length;
        }

        std::cout << "FLEE: Fleeing in opposite direction of average obstacle: "
                  << fleeDirection.x << "," << fleeDirection.y << std::endl;
    }
    else
    {
        // If no obstacles detected, flee in a random direction
        float angle = (rand() % 360) * 3.14159f / 180.0f;
        fleeDirection = sf::Vector2f(std::cos(angle), std::sin(angle));
        std::cout << "FLEE: No obstacles found, using random flee direction" << std::endl;
    }

    // Set velocity to flee
    float fleeSpeed = 150.0f;
    monsterKinematic.velocity = fleeDirection * fleeSpeed;

    // Update orientation to match flee direction
    monsterKinematic.orientation = std::atan2(fleeDirection.y, fleeDirection.x) * 180.0f / 3.14159f;

    // Check if the flee direction is safe
    sf::Vector2f proposedPosition = monsterKinematic.position + monsterKinematic.velocity * deltaTime;
    if (checkCollision(proposedPosition))
    {
        // Try several different angles if the original flee direction is blocked
        std::cout << "FLEE: Initial flee direction blocked, trying alternatives" << std::endl;
        bool foundSafePath = false;

        for (int angleOffset = 30; angleOffset < 360 && !foundSafePath; angleOffset += 30)
        {
            // Try clockwise and counterclockwise offsets
            for (int sign : {1, -1})
            {
                float radian = std::atan2(fleeDirection.y, fleeDirection.x) + sign * angleOffset * 3.14159f / 180.0f;
                sf::Vector2f newDir(std::cos(radian), std::sin(radian));

                sf::Vector2f testPosition = monsterKinematic.position + newDir * fleeSpeed * deltaTime;
                if (!checkCollision(testPosition))
                {
                    fleeDirection = newDir;
                    monsterKinematic.velocity = fleeDirection * fleeSpeed;
                    monsterKinematic.orientation = radian * 180.0f / 3.14159f;
                    foundSafePath = true;
                    std::cout << "FLEE: Found safe path at angle offset " << (sign * angleOffset) << std::endl;
                    break;
                }
            }
        }

        if (!foundSafePath)
        {
            // If all directions are blocked, move slower
            std::cout << "FLEE: All directions blocked, reducing speed" << std::endl;
            monsterKinematic.velocity *= 0.5f;
        }
    }

    // Apply movement
    monsterKinematic.update(deltaTime);
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