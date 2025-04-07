/**
 * @file BehaviorTree.cpp
 * @brief Implementation of the Behavior Tree classes.
 *
 * Author: Miles Hollifield
 * Date: 4/7/2025
 */

#include "headers/BehaviorTree.h"
#include <iostream>

// InverterNode implementation
BehaviorStatus InverterNode::tick()
{
    BehaviorStatus status = child->tick();

    if (status == BehaviorStatus::SUCCESS)
    {
        return BehaviorStatus::FAILURE;
    }
    else if (status == BehaviorStatus::FAILURE)
    {
        return BehaviorStatus::SUCCESS;
    }

    // Pass through RUNNING status
    return status;
}

// RepeatNode implementation
BehaviorStatus RepeatNode::tick()
{
    // If we've reached the maximum repeat count, succeed
    if (maxRepeatCount > 0 && repeatCount >= maxRepeatCount)
    {
        return BehaviorStatus::SUCCESS;
    }

    // Tick the child
    BehaviorStatus status = child->tick();

    // If the child is still running, we're still running
    if (status == BehaviorStatus::RUNNING)
    {
        return BehaviorStatus::RUNNING;
    }

    // If the child succeeded or failed, increment the count and reset the child
    if (status == BehaviorStatus::SUCCESS || status == BehaviorStatus::FAILURE)
    {
        repeatCount++;
        child->reset();

        // If we've reached the maximum repeat count, succeed
        if (maxRepeatCount > 0 && repeatCount >= maxRepeatCount)
        {
            return BehaviorStatus::SUCCESS;
        }

        // Otherwise, we're still running
        return BehaviorStatus::RUNNING;
    }

    // This should never happen
    return BehaviorStatus::FAILURE;
}

void RepeatNode::reset()
{
    repeatCount = 0;
    child->reset();
}

// SequenceNode implementation
BehaviorStatus SequenceNode::tick()
{
    // Continue from where we left off
    while (currentChild < children.size())
    {
        BehaviorStatus status = children[currentChild]->tick();

        // If the child is still running or failed, return that status
        if (status != BehaviorStatus::SUCCESS)
        {
            return status;
        }

        // Child succeeded, move to next child
        currentChild++;
    }

    // All children succeeded
    currentChild = 0; // Reset for next time
    return BehaviorStatus::SUCCESS;
}

void SequenceNode::reset()
{
    currentChild = 0;
    for (auto &child : children)
    {
        child->reset();
    }
}

// SelectorNode implementation
BehaviorStatus SelectorNode::tick()
{
    // Continue from where we left off
    while (currentChild < children.size())
    {
        BehaviorStatus status = children[currentChild]->tick();

        // If the child is still running or succeeded, return that status
        if (status != BehaviorStatus::FAILURE)
        {
            return status;
        }

        // Child failed, try the next one
        currentChild++;
    }

    // All children failed
    currentChild = 0; // Reset for next time
    return BehaviorStatus::FAILURE;
}

void SelectorNode::reset()
{
    currentChild = 0;
    for (auto &child : children)
    {
        child->reset();
    }
}

// RandomSelectorNode implementation
BehaviorStatus RandomSelectorNode::tick()
{
    // If we haven't selected a child yet, or we're not running anymore, select a new one
    if (selectedChild == -1 || lastStatus != BehaviorStatus::RUNNING)
    {
        // Select a random child
        selectedChild = rand() % children.size();
    }

    // Execute the selected child
    lastStatus = children[selectedChild]->tick();
    return lastStatus;
}

void RandomSelectorNode::reset()
{
    selectedChild = -1;
    lastStatus = BehaviorStatus::FAILURE;
    for (auto &child : children)
    {
        child->reset();
    }
}

// ParallelNode implementation
BehaviorStatus ParallelNode::tick()
{
    int successCount = 0;
    int failureCount = 0;

    // Execute all children
    for (size_t i = 0; i < children.size(); i++)
    {
        // Only tick if the child isn't already in a terminal state
        if (childStatuses[i] == BehaviorStatus::RUNNING)
        {
            childStatuses[i] = children[i]->tick();
        }

        // Count successes and failures
        if (childStatuses[i] == BehaviorStatus::SUCCESS)
        {
            successCount++;
        }
        else if (childStatuses[i] == BehaviorStatus::FAILURE)
        {
            failureCount++;
        }
    }

    // Check if we've met the success or failure policy
    if (successPolicy > 0 && successCount >= successPolicy)
    {
        return BehaviorStatus::SUCCESS;
    }
    if (failurePolicy > 0 && failureCount >= failurePolicy)
    {
        return BehaviorStatus::FAILURE;
    }

    // If we're here, we're still running
    return BehaviorStatus::RUNNING;
}

void ParallelNode::reset()
{
    for (size_t i = 0; i < children.size(); i++)
    {
        children[i]->reset();
        childStatuses[i] = BehaviorStatus::RUNNING;
    }
}

// BehaviorTree implementation
BehaviorStatus BehaviorTree::tick()
{
    if (!rootNode)
    {
        return BehaviorStatus::FAILURE;
    }

    return rootNode->tick();
}

void BehaviorTree::reset()
{
    if (rootNode)
    {
        rootNode->reset();
    }
}