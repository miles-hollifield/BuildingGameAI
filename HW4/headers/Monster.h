/**
 * @file Monster.h
 * @brief Defines the Monster class for HW4.
 *
 * Resources Used:
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - Book: "Game AI Pro" edited by Steve Rabin
 *
 * Author: Miles Hollifield
 * Date: 4/7/2025
 */

#ifndef MONSTER_H
#define MONSTER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <deque>
#include <memory>
#include "headers/Kinematic.h"
#include "headers/Arrive.h"
#include "headers/Align.h"
#include "headers/Graph.h"
#include "headers/Environment.h"

// Forward declarations
class BehaviorTree;
class DecisionTree;
class EnvironmentState;

/**
 * @class Monster
 * @brief A monster entity that can be controlled by a behavior tree or decision tree
 */
class Monster
{
public:
    enum class ControlType
    {
        BEHAVIOR_TREE,
        DECISION_TREE
    };

    /**
     * @brief Constructor
     * @param startPosition Initial position of the monster
     * @param texture Texture for the monster's sprite
     * @param environment Reference to the environment
     * @param graph Reference to the navigation graph
     * @param color Optional color to apply to the sprite (defaults to red)
     */
    Monster(sf::Vector2f startPosition, sf::Texture &texture, Environment &environment, Graph &graph,
            sf::Color color = sf::Color::Red);

    /**
     * @brief Set the control type for the monster
     * @param type Type of control (behavior tree or decision tree)
     */
    void setControlType(ControlType type);

    /**
     * @brief Set the player's kinematic for tracking
     * @param playerKinematic Reference to the player's kinematic
     */
    void setPlayerKinematic(const Kinematic &playerKinematic);

    /**
     * @brief Set the monster's behavior tree
     * @param tree Pointer to the behavior tree
     */
    void setBehaviorTree(std::shared_ptr<BehaviorTree> tree);

    /**
     * @brief Set the monster's decision tree
     * @param tree Pointer to the decision tree
     */
    void setDecisionTree(std::shared_ptr<DecisionTree> tree);

    /**
     * @brief Reset the monster to its starting position
     */
    void reset();

    /**
     * @brief Update the monster's position and behavior
     * @param deltaTime Time since last update
     * @return True if the monster has caught the player
     */
    bool update(float deltaTime);

    /**
     * @brief Draw the monster and any visual debugging information
     * @param window Window to draw to
     */
    void draw(sf::RenderWindow &window);

    /**
     * @brief Get the monster's position
     * @return Current position vector
     */
    sf::Vector2f getPosition() const;

    /**
     * @brief Get the monster's kinematic data
     * @return Reference to kinematic data
     */
    const Kinematic &getKinematic() const;

    /**
     * @brief Get the environment reference
     * @return Reference to the environment
     */
    const Environment &getEnvironment() const { return environment; }

    /**
     * @brief Get the player's kinematic data
     * @return Const reference to the player's kinematic data
     */
    const Kinematic &getPlayerKinematic() const { return *playerKinematic; }

    /**
     * @brief Execute a specific action
     * @param action The action to execute
     * @param deltaTime Time since last update
     */
    void executeAction(const std::string &action, float deltaTime);

    /**
     * @brief Check if the monster has caught the player
     * @return True if the monster has caught the player
     */
    bool hasCaughtPlayer() const;

    /**
     * @brief Check if the monster has line of sight to a target
     * @param target Target position to check
     * @return True if there's a clear line of sight
     */
    bool hasLineOfSightTo(const sf::Vector2f &target) const;

    /**
     * @brief Record the current state and action to a file
     * @param outputFile Output stream to write to
     */
    void recordStateAction(std::ofstream &outputFile);

    /**
     * @brief Set and get the current delta time (for behavior tree actions)
     */
    void setDeltaTime(float deltaTime) { currentDeltaTime = deltaTime; }
    float getDeltaTime() const { return currentDeltaTime; }

    /**
     * @brief Set the monster's orientation
     * @param orientation New orientation in degrees
     */
    void setOrientation(float orientation) { monsterKinematic.orientation = orientation; }

    /**
     * @brief Get whether the monster is currently dancing
     */
    bool getIsDancing() const { return isDancing; }

    /**
     * @brief Get the current path count (for decision tree learning)
     * @return Number of waypoints in the current path
     */
    int getPathCount() const { return currentPath.size(); }

    /**
     * @brief Check if the monster has an active path
     * @return True if the monster has a non-empty path
     */
    bool hasActivePath() const { return !currentPath.empty(); }

    /**
     * @brief Get the time spent in the current action
     * @return Time in seconds
     */
    float getTimeInCurrentAction() const { return timeInCurrentAction; }

    /**
     * @brief Get the current action
     * @return String representation of the current action
     */
    std::string getCurrentAction() const { return currentAction; }
    
    /**
     * @brief Create an environment state for this monster
     * @return Shared pointer to an environment state object
     */
    std::shared_ptr<EnvironmentState> createEnvironmentState();

private:
    // Entity data
    Kinematic monsterKinematic;
    sf::Sprite sprite;
    sf::Vector2f startPosition;

    // Movement behaviors
    Arrive arriveBehavior;
    Align alignBehavior;

    // References to environment and graph
    Environment &environment;
    Graph &navigationGraph;

    // Player reference
    const Kinematic *playerKinematic;

    // Path following
    std::vector<sf::Vector2f> currentPath;
    int currentWaypointIndex;

    // Control
    ControlType controlType;
    std::shared_ptr<BehaviorTree> behaviorTree;
    std::shared_ptr<DecisionTree> decisionTree;
    float currentDeltaTime;

    // State tracking
    std::string currentAction;
    float timeInCurrentAction;
    float catchDistance;

    // Special behaviors
    std::vector<sf::Vector2f> dancePath;
    bool isDancing;
    float danceTimer;
    int dancePhase;

    // Helper methods
    void pathfindToPlayer();
    void wander(float deltaTime);
    void followPath(float deltaTime);
    void doDance(float deltaTime);
    void updateSprite();
    void flee(float deltaTime);

    // Collision handling
    bool checkCollision(sf::Vector2f proposedPosition) const;
    sf::Vector2f findValidMovement(sf::Vector2f currentPos, sf::Vector2f proposedPos) const;

    // Breadcrumb trail for visualization
    std::deque<sf::CircleShape> breadcrumbs;
    int breadcrumbCounter;
    sf::Color breadcrumbColor;
    static constexpr int BREADCRUMB_INTERVAL = 120; // More frequent breadcrumbs than before
    static constexpr int MAX_BREADCRUMBS = 30; // Store more breadcrumbs
    void dropBreadcrumb();
};

#endif // MONSTER_H