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
#include <vector>
#include <SFML/System.hpp>
#include "Kinematic.h" // For access to the agent's kinematic data
#include "Environment.h" // For environment state checking

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
 * @class RandomDecisionNode
 * @brief Decision node that makes a random choice between multiple options
 */
class RandomDecisionNode : public DecisionNode
{
public:
    /**
     * @brief Constructor for random decision node
     * @param name Human-readable name for the node
     */
    RandomDecisionNode(const std::string &name)
    {
        nodeName = "Random: " + name;
    }

    /**
     * @brief Add a child node with a specific weight
     * @param child Node to add
     * @param weight Relative probability of selecting this child
     */
    void addChild(std::shared_ptr<DecisionNode> child, float weight = 1.0f)
    {
        children.push_back(child);
        weights.push_back(weight);
        totalWeight += weight;
    }

    /**
     * @brief Make a random decision by selecting a child based on weights
     * @return Result of the selected child
     */
    std::string makeDecision() override
    {
        if (children.empty())
        {
            return "Idle"; // Default action if no children
        }

        // Generate a random value
        float randomValue = static_cast<float>(rand()) / RAND_MAX * totalWeight;

        // Find which child to select
        float cumulativeWeight = 0.0f;
        for (size_t i = 0; i < children.size(); i++)
        {
            cumulativeWeight += weights[i];
            if (randomValue <= cumulativeWeight)
            {
                return children[i]->makeDecision();
            }
        }

        // Fallback to the last child
        return children.back()->makeDecision();
    }

private:
    std::vector<std::shared_ptr<DecisionNode>> children;
    std::vector<float> weights;
    float totalWeight = 0.0f;
};

/**
 * @class PriorityNode
 * @brief Decision node that selects the first child whose condition is true
 */
class PriorityNode : public DecisionNode
{
public:
    /**
     * @brief Constructor for priority node
     * @param name Human-readable name for the node
     */
    PriorityNode(const std::string &name)
    {
        nodeName = "Priority: " + name;
    }

    /**
     * @brief Add a child with a condition
     * @param condition Function that returns true if this child should be selected
     * @param child Node to evaluate if condition is true
     * @param conditionName Human-readable name for the condition
     */
    void addChild(std::function<bool()> condition, std::shared_ptr<DecisionNode> child, const std::string &conditionName)
    {
        conditions.push_back(condition);
        children.push_back(child);
        conditionNames.push_back(conditionName);
    }

    /**
     * @brief Make a decision by evaluating conditions in order
     * @return Result of the first child whose condition is true
     */
    std::string makeDecision() override
    {
        for (size_t i = 0; i < children.size(); i++)
        {
            if (conditions[i]())
            {
                return children[i]->makeDecision();
            }
        }

        // If no condition is true, return a default action
        return "Idle";
    }

private:
    std::vector<std::function<bool()>> conditions;
    std::vector<std::shared_ptr<DecisionNode>> children;
    std::vector<std::string> conditionNames;
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
    EnvironmentState(const Kinematic &character, const Environment &env);

    /**
     * @brief Update the state based on current conditions
     */
    void update();

    // State parameters for decision making
    bool isNearObstacle(float threshold = 50.0f) const;
    bool isMovingFast(float threshold = 100.0f) const;
    bool isInRoom(int roomId) const;
    float getDistanceToTarget(const sf::Vector2f &target) const;
    bool hasBeenInCurrentState(float seconds) const;
    bool hasReachedWaypoint() const;
    bool hasCompletedPath() const;
    bool isPathBlocked() const;
    bool canSeeTarget(const sf::Vector2f &target) const;
    bool isNearWall() const;
    bool isInCenterOfRoom() const;
    bool hasLineOfSightTo(const sf::Vector2f &target) const;
    bool isMovingTowards(const sf::Vector2f &target) const;
    bool isIdleForTooLong(float threshold = 3.0f) const;
    bool shouldChangeTarget() const;
    
    // Getters for state variables
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getVelocity() const { return velocity; }
    float getSpeed() const { return speed; }
    
    // Set target for relative conditions
    void setTarget(const sf::Vector2f &target);
    
    // Reset timer for state duration tracking
    void resetStateTimer();

private:
    const Kinematic &character;
    const Environment &environment;

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
    sf::Vector2f currentTarget;
    sf::Clock idleTimer;
    bool isIdle;
    
    // Helper methods for state calculation
    void findNearestObstacle();
    int determineCurrentRoom();
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
     * @brief Build a complex decision tree for controlling character movement
     * @param targets List of potential target positions
     */
    void buildComplexTree(const std::vector<sf::Vector2f> &targets);

    /**
     * @brief Print the decision tree structure for debugging
     * @param node Starting node (default: root)
     * @param depth Current depth for indentation
     * @return String representation of the tree
     */
    std::string printTree(std::shared_ptr<DecisionNode> node = nullptr, int depth = 0) const;

private:
    std::shared_ptr<DecisionNode> rootNode;
    EnvironmentState &environmentState;
    
    // Helper method to create nodes for specific targets
    std::shared_ptr<DecisionNode> createTargetSubtree(const sf::Vector2f &target);
};

#endif // DECISION_TREE_H