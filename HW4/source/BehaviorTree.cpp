/**
 * @file BehaviorTree.cpp
 * @brief Implementation of the Behavior Tree classes.
 *
 * Author: Miles Hollifield
 * Date: 4/7/2025
 */

#include "headers/BehaviorTree.h"
#include <iostream>

// SequenceNode implementation
BehaviorStatus SequenceNode::tick()
{
    // If not running, start from the beginning
    if (!isRunning)
    {
        currentChild = 0;
    }

    // Continue from where we left off
    while (currentChild < children.size())
    {
        BehaviorStatus status = children[currentChild]->tick();

        if (status == BehaviorStatus::RUNNING)
        {
            isRunning = true;
            return BehaviorStatus::RUNNING;
        }
        else if (status == BehaviorStatus::FAILURE)
        {
            // Child failed, entire sequence fails
            isRunning = false;
            return BehaviorStatus::FAILURE;
        }

        // Child succeeded, move to next child
        currentChild++;
    }

    // All children succeeded
    isRunning = false;
    return BehaviorStatus::SUCCESS;
}

void SequenceNode::reset()
{
    currentChild = 0;
    isRunning = false;
    for (auto &child : children)
    {
        child->reset();
    }
}

// SelectorNode implementation
BehaviorStatus SelectorNode::tick()
{
    std::cout << "SELECTOR [" << nodeName << "]: Starting tick, currentChild = " 
              << currentChild << ", isRunning = " << (isRunning ? "true" : "false") << std::endl;
              
    // If not running, start from the beginning
    if (!isRunning) {
        currentChild = 0;
    }
    
    // Continue from where we left off
    while (currentChild < children.size())
    {
        std::cout << "SELECTOR [" << nodeName << "]: Trying child " << currentChild 
                  << " (" << children[currentChild]->getName() << ")" << std::endl;
                  
        BehaviorStatus status = children[currentChild]->tick();
        
        std::cout << "SELECTOR [" << nodeName << "]: Child " << currentChild 
                  << " returned status " << (status == BehaviorStatus::SUCCESS ? "SUCCESS" : 
                  (status == BehaviorStatus::FAILURE ? "FAILURE" : "RUNNING")) << std::endl;

        if (status == BehaviorStatus::RUNNING)
        {
            isRunning = true;
            return BehaviorStatus::RUNNING;
        }
        else if (status == BehaviorStatus::SUCCESS)
        {
            // Child succeeded, entire selector succeeds
            isRunning = false;
            return BehaviorStatus::SUCCESS;
        }

        // Child failed, try the next one
        currentChild++;
    }

    // All children failed
    std::cout << "SELECTOR [" << nodeName << "]: All children failed" << std::endl;
    isRunning = false;
    return BehaviorStatus::FAILURE;
}

void SelectorNode::reset()
{
    currentChild = 0;
    isRunning = false;
    for (auto &child : children)
    {
        child->reset();
    }
}

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
        // Reset all children for next time
        for (size_t i = 0; i < children.size(); i++)
        {
            children[i]->reset();
            childStatuses[i] = BehaviorStatus::RUNNING;
        }
        return BehaviorStatus::SUCCESS;
    }

    if (failurePolicy > 0 && failureCount >= failurePolicy)
    {
        // Reset all children for next time
        for (size_t i = 0; i < children.size(); i++)
        {
            children[i]->reset();
            childStatuses[i] = BehaviorStatus::RUNNING;
        }
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