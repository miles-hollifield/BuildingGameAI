/**
 * @file DecisionTree.h
 * @brief Defines the Decision Tree classes for agent decision making.
 *
 * Resources Used:
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - Book: "Game AI Pro" edited by Steve Rabin
 *
 * Author: Miles Hollifield
 * Date: 4/7/2025
 */

#ifndef DECISION_TREE_H
#define DECISION_TREE_H

#include <memory>
#include <functional>
#include <string>
#include <SFML/System.hpp>
#include "Kinematic.h" // For access to the agent's kinematic data

/**
 * @class DecisionNode
 * @brief Abstract base class for all decision tree nodes
 */
class DecisionNode
{
public:
    virtual ~DecisionNode() = default;

    /**
     * @brief Pure virtual function to make a decision
     * @return A string representing the action to take
     */
    virtual std::string makeDecision() = 0;

    /**
     * @brief Get the name of the decision node for debugging
     * @return Name of the decision node
     */
    std::string getName() const { return nodeName; }

protected:
    std::string nodeName = "Unnamed Node";
};

/**
 * @class ActionNode
 * @brief Leaf node that represents an action to take
 */
class ActionNode : public DecisionNode
{
public:
    /**
     * @brief Constructor for action node
     * @param actionName The name of the action to return
     */
    ActionNode(const std::string &actionName)
    {
        actionValue = actionName;
        nodeName = "Action: " + actionName;
    }

    /**
     * @brief Return the action
     * @return The action name
     */
    std::string makeDecision() override
    {
        return actionValue;
    }

private:
    std::string actionValue;
};

/**
 * @class DecisionBranch
 * @brief Decision node that branches based on a condition
 */
class DecisionBranch : public DecisionNode
{
public:
    /**
     * @brief Constructor for decision branch
     * @param condition Function that returns true or false
     * @param trueNode Node to evaluate if condition is true
     * @param falseNode Node to evaluate if condition is false
     * @param conditionName Human-readable name for the condition
     */
    DecisionBranch(
        std::function<bool()> condition,
        std::shared_ptr<DecisionNode> trueNode,
        std::shared_ptr<DecisionNode> falseNode,
        const std::string &conditionName) : condition(condition), trueNode(trueNode), falseNode(falseNode)
    {
        nodeName = "Decision: " + conditionName;
    }

    /**
     * @brief Make a decision by evaluating the condition
     * @return Result of the selected branch
     */
    std::string makeDecision() override
    {
        if (condition())
        {
            return trueNode->makeDecision();
        }
        else
        {
            return falseNode->makeDecision();
        }
    }

private:
    std::function<bool()> condition;
    std::shared_ptr<DecisionNode> trueNode;
    std::shared_ptr<DecisionNode> falseNode;
};

/**
 * @class EnvironmentState
 * @brief Stores and computes environmental state information for use in decisions
 */
class EnvironmentState
{
public:
    /**
     * @brief Constructor
     * @param character Reference to the character's kinematic data
     * @param environment Reference to the environment
     */
    EnvironmentState(const Kinematic &character);

    /**
     * @brief Update the state based on current conditions
     */
    void update();

    // State accessors
    bool isNearObstacle(float threshold = 50.0f) const;
    bool isMovingFast(float threshold = 150.0f) const;
    bool isInRoom(int roomId) const;
    float getDistanceToTarget(const sf::Vector2f &target) const;
    bool hasBeenInCurrentState(float seconds) const;
    bool hasReachedWaypoint() const;
    bool hasCompletedPath() const;
    bool isPathBlocked() const;
    bool canSeeTarget(const sf::Vector2f &target) const;

private:
    const Kinematic &characterKinematic;

    // Cached state variables
    sf::Vector2f position;
    sf::Vector2f velocity;
    float speed;
    float distanceToNearestObstacle;
    int currentRoom;
    sf::Clock stateTimer;
    bool reachedWaypoint;
    bool completedPath;
    bool pathBlocked;
};

/**
 * @class DecisionTree
 * @brief Main class for constructing and executing decision trees
 */
class DecisionTree
{
public:
    /**
     * @brief Constructor
     * @param state Reference to environment state
     */
    DecisionTree(EnvironmentState &state);

    /**
     * @brief Set the root node of the decision tree
     * @param root Root node of the decision tree
     */
    void setRootNode(std::shared_ptr<DecisionNode> root);

    /**
     * @brief Make a decision by evaluating the tree
     * @return String representing the decided action
     */
    virtual std::string makeDecision();

    /**
     * @brief Build a sample decision tree for controlling character movement
     * @param targets List of potential target positions
     */
    void buildSampleTree(const std::vector<sf::Vector2f> &targets);

private:
    std::shared_ptr<DecisionNode> rootNode;
    EnvironmentState &environmentState;
};

#endif // DECISION_TREE_H