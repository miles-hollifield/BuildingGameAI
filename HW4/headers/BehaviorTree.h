/**
 * @file BehaviorTree.h
 * @brief Defines the Behavior Tree classes for agent decision making.
 *
 * Resources Used:
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - Book: "Game AI Pro" edited by Steve Rabin
 * - Book: "Behavior Trees in Robotics and AI" by Michele Colledanchise and Petter Ögren
 *
 * Author: Miles Hollifield
 * Date: 4/7/2025
 */

#ifndef BEHAVIOR_TREE_H
#define BEHAVIOR_TREE_H

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <SFML/System.hpp>
#include "headers/Kinematic.h"

/**
 * @brief Return status of behavior tree nodes
 */
enum class BehaviorStatus
{
    SUCCESS, // Node executed successfully
    FAILURE, // Node execution failed
    RUNNING  // Node is still executing
};

/**
 * @struct BehaviorState
 * @brief Helper struct to maintain state between ticks
 */
struct BehaviorState
{
    bool initialized = false;
    float timer = 0.0f;
    int phase = 0;
    int counter = 0;
    std::unordered_map<std::string, float> floatParams;
    std::unordered_map<std::string, int> intParams;
    std::unordered_map<std::string, bool> boolParams;

    void reset()
    {
        initialized = false;
        timer = 0.0f;
        phase = 0;
        counter = 0;
        floatParams.clear();
        intParams.clear();
        boolParams.clear();
    }
};

/**
 * @class BehaviorNode
 * @brief Abstract base class for all behavior tree nodes
 */
class BehaviorNode
{
public:
    virtual ~BehaviorNode() = default;

    /**
     * @brief Pure virtual function to tick/update the node
     * @return Status of execution
     */
    virtual BehaviorStatus tick() = 0;

    /**
     * @brief Reset the node's internal state
     */
    virtual void reset() = 0;

    /**
     * @brief Get the name of this node (for debugging)
     */
    std::string getName() const { return nodeName; }

protected:
    std::string nodeName = "Unnamed Node";
};

/**
 * @class BehaviorActionNode
 * @brief Leaf node that represents an action to perform
 */
class BehaviorActionNode : public BehaviorNode
{
public:
    /**
     * @brief Constructor for action node
     * @param action Function that performs the action and returns status
     * @param name Name of the action (for debugging)
     */
    BehaviorActionNode(std::function<BehaviorStatus()> action, const std::string &name)
        : action(action)
    {
        nodeName = "Action: " + name;
    }

    /**
     * @brief Execute the action
     * @return Status of the action execution
     */
    BehaviorStatus tick() override
    {
        return action();
    }

    /**
     * @brief Reset the node's state
     */
    void reset() override
    {
        // Most action nodes are stateless, so nothing to reset
    }

private:
    std::function<BehaviorStatus()> action;
};

/**
 * @class ConditionNode
 * @brief Leaf node that checks a condition
 */
class ConditionNode : public BehaviorNode
{
public:
    /**
     * @brief Constructor for condition node
     * @param condition Function that checks a condition
     * @param name Name of the condition (for debugging)
     */
    ConditionNode(std::function<bool()> condition, const std::string &name)
        : condition(condition)
    {
        nodeName = "Condition: " + name;
    }

    /**
     * @brief Check the condition
     * @return SUCCESS if condition is true, FAILURE otherwise
     */
    BehaviorStatus tick() override
    {
        return condition() ? BehaviorStatus::SUCCESS : BehaviorStatus::FAILURE;
    }

    /**
     * @brief Reset the node's state
     */
    void reset() override
    {
        // Condition nodes are stateless, so nothing to reset
    }

private:
    std::function<bool()> condition;
};

/**
 * @class SequenceNode
 * @brief Composite node that executes children in sequence until one fails
 */
class SequenceNode : public BehaviorNode
{
public:
    /**
     * @brief Constructor for sequence node
     * @param name Name of the sequence (for debugging)
     */
    SequenceNode(const std::string &name = "Sequence")
    {
        nodeName = name;
    }

    /**
     * @brief Add a child node to the sequence
     * @param child Child node to add
     */
    void addChild(std::shared_ptr<BehaviorNode> child)
    {
        children.push_back(child);
    }

    /**
     * @brief Execute children in sequence
     * @return SUCCESS if all children succeeded, FAILURE if any child failed, RUNNING if a child is still running
     */
    BehaviorStatus tick() override;

    /**
     * @brief Reset the node's state and all children
     */
    void reset() override;

private:
    std::vector<std::shared_ptr<BehaviorNode>> children;
    size_t currentChild = 0;
    bool isRunning = false;
};

/**
 * @class SelectorNode
 * @brief Composite node that tries children in order until one succeeds
 */
class SelectorNode : public BehaviorNode
{
public:
    /**
     * @brief Constructor for selector node
     * @param name Name of the selector (for debugging)
     */
    SelectorNode(const std::string &name = "Selector")
    {
        nodeName = name;
    }

    /**
     * @brief Add a child node to the selector
     * @param child Child node to add
     */
    void addChild(std::shared_ptr<BehaviorNode> child)
    {
        children.push_back(child);
    }

    /**
     * @brief Try children in order until one succeeds
     * @return SUCCESS if any child succeeded, FAILURE if all children failed, RUNNING if a child is still running
     */
    BehaviorStatus tick() override;

    /**
     * @brief Reset the node's state and all children
     */
    void reset() override;

private:
    std::vector<std::shared_ptr<BehaviorNode>> children;
    size_t currentChild = 0;
    bool isRunning = false;
};

/**
 * @class DecoratorNode
 * @brief Base class for decorators that modify the behavior of a single child
 */
class DecoratorNode : public BehaviorNode
{
public:
    /**
     * @brief Constructor for decorator node
     * @param child Child node to decorate
     * @param name Name of the decorator (for debugging)
     */
    DecoratorNode(std::shared_ptr<BehaviorNode> child, const std::string &name)
        : child(child)
    {
        nodeName = name;
    }

    /**
     * @brief Reset the node's state and child
     */
    void reset() override
    {
        child->reset();
    }

protected:
    std::shared_ptr<BehaviorNode> child;
};

/**
 * @class InverterNode
 * @brief Decorator that inverts the result of its child
 */
class InverterNode : public DecoratorNode
{
public:
    /**
     * @brief Constructor for inverter node
     * @param child Child node to invert
     */
    InverterNode(std::shared_ptr<BehaviorNode> child)
        : DecoratorNode(child, "Inverter")
    {
    }

    /**
     * @brief Invert the result of the child
     * @return FAILURE if child succeeded, SUCCESS if child failed, RUNNING if child is still running
     */
    BehaviorStatus tick() override;
};

/**
 * @class RepeatNode
 * @brief Decorator that repeats its child a specified number of times
 */
class RepeatNode : public DecoratorNode
{
public:
    /**
     * @brief Constructor for repeat node
     * @param child Child node to repeat
     * @param repeatCount Number of times to repeat (0 for infinity)
     */
    RepeatNode(std::shared_ptr<BehaviorNode> child, int repeatCount = 0)
        : DecoratorNode(child, "Repeat"), maxRepeatCount(repeatCount)
    {
    }

    /**
     * @brief Repeat the child
     * @return SUCCESS if completed all repetitions, RUNNING otherwise
     */
    BehaviorStatus tick() override;

    /**
     * @brief Reset the node's state and child
     */
    void reset() override;

private:
    int repeatCount = 0;
    int maxRepeatCount;
};

/**
 * @class RandomSelectorNode
 * @brief Composite node that randomly selects a child to execute
 */
class RandomSelectorNode : public BehaviorNode
{
public:
    /**
     * @brief Constructor for random selector node
     * @param name Name of the random selector (for debugging)
     */
    RandomSelectorNode(const std::string &name = "Random Selector")
    {
        nodeName = name;
    }

    /**
     * @brief Add a child node to the random selector
     * @param child Child node to add
     */
    void addChild(std::shared_ptr<BehaviorNode> child)
    {
        children.push_back(child);
    }

    /**
     * @brief Randomly select a child to execute
     * @return Status of the selected child
     */
    BehaviorStatus tick() override;

    /**
     * @brief Reset the node's state and all children
     */
    void reset() override;

private:
    std::vector<std::shared_ptr<BehaviorNode>> children;
    int selectedChild = -1;
    BehaviorStatus lastStatus = BehaviorStatus::FAILURE;
};

/**
 * @class ParallelNode
 * @brief Composite node that executes all children simultaneously
 */
class ParallelNode : public BehaviorNode
{
public:
    /**
     * @brief Constructor for parallel node
     * @param successPolicy How many children need to succeed for the node to succeed
     * @param failurePolicy How many children need to fail for the node to fail
     * @param name Name of the parallel node (for debugging)
     */
    ParallelNode(int successPolicy, int failurePolicy, const std::string &name = "Parallel")
        : successPolicy(successPolicy), failurePolicy(failurePolicy)
    {
        nodeName = name;
    }

    /**
     * @brief Add a child node to the parallel node
     * @param child Child node to add
     */
    void addChild(std::shared_ptr<BehaviorNode> child)
    {
        children.push_back(child);
        childStatuses.push_back(BehaviorStatus::FAILURE);
    }

    /**
     * @brief Execute all children simultaneously
     * @return SUCCESS if success policy met, FAILURE if failure policy met, RUNNING otherwise
     */
    BehaviorStatus tick() override;

    /**
     * @brief Reset the node's state and all children
     */
    void reset() override;

private:
    std::vector<std::shared_ptr<BehaviorNode>> children;
    std::vector<BehaviorStatus> childStatuses;
    int successPolicy;
    int failurePolicy;
};

/**
 * @class BehaviorTree
 * @brief Main class for constructing and executing behavior trees
 */
class BehaviorTree
{
public:
    /**
     * @brief Constructor
     */
    BehaviorTree() : rootNode(nullptr) {}

    /**
     * @brief Set the root node of the behavior tree
     * @param root Root node of the behavior tree
     */
    void setRootNode(std::shared_ptr<BehaviorNode> root)
    {
        rootNode = root;
    }

    /**
     * @brief Tick/update the behavior tree
     * @return Status of the root node execution
     */
    BehaviorStatus tick();

    /**
     * @brief Reset the behavior tree
     */
    void reset();

private:
    std::shared_ptr<BehaviorNode> rootNode;
};

#endif // BEHAVIOR_TREE_H