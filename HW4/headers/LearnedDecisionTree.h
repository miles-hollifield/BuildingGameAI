/**
 * @file LearnedDecisionTree.h
 * @brief A specialized decision tree that uses a learned model.
 *
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - AI Tools: OpenAI's ChatGPT
 *
 * OpenAI's ChatGPT was used to suggest a template header file for LearnedDecisionTree's
 * implementation. The following prompt was used: "Create a template header file
 * for a learned decision tree in C++ that uses a decision tree node structure for classification."
 * The code provided by ChatGPT was modified to fit the context of the project.
 *
 * Author: Miles Hollifield
 * Date: 4/20/2025
 */

#ifndef LEARNED_DECISION_TREE_H
#define LEARNED_DECISION_TREE_H

#include "headers/DecisionTree.h"
#include "headers/DTLearning.h"
#include "headers/Monster.h"
#include <memory>
#include <vector>
#include <string>
#include <iostream>

/**
 * @class LearnedDecisionTree
 * @brief A decision tree that uses a learned model for decision making.
 */
class LearnedDecisionTree : public DecisionTree
{
public:
    /**
     * @brief Constructor
     * @param root The root node of the learned decision tree
     * @param monster Reference to the monster this tree controls
     */
    LearnedDecisionTree(std::shared_ptr<DTNode> root, Monster &monster)
        : DecisionTree(*monster.createEnvironmentState()),
          dtRoot(root),
          monster(monster) {}

    /**
     * @brief Make a decision based on the current state
     * @return String representing the decided action
     */
    std::string makeDecision() override
    {
        if (!dtRoot)
        {
            std::cerr << "LearnedDecisionTree has no root node!" << std::endl;
            return "Idle"; // Default action if no tree is defined
        }

        // Get the current state vector
        std::vector<std::string> stateVector = getStateVector();

        // Use the learned tree to make a decision
        std::string decision = dtRoot->classify(stateVector);

        // Log the decision for debugging
        std::cout << "Learned DT decision: " << decision << std::endl;

        return decision;
    }

private:
    std::shared_ptr<DTNode> dtRoot;
    Monster &monster;

    /**
     * @brief Convert the current state to a vector of discrete attribute values
     * @return Vector of state attributes for decision tree classification
     */
    std::vector<std::string> getStateVector()
    {
        std::vector<std::string> stateVector;

        // Get current monster state
        const Kinematic &monsterKinematic = monster.getKinematic();
        const Kinematic &playerKinematic = monster.getPlayerKinematic();

        // Distance to player
        float dx = playerKinematic.position.x - monsterKinematic.position.x;
        float dy = playerKinematic.position.y - monsterKinematic.position.y;
        float distanceToPlayer = std::sqrt(dx * dx + dy * dy);

        if (distanceToPlayer < 30.0f)
        {
            stateVector.push_back("very_near");
        }
        else if (distanceToPlayer < 80.0f)
        {
            stateVector.push_back("near");
        }
        else if (distanceToPlayer < 200.0f)
        {
            stateVector.push_back("medium");
        }
        else
        {
            stateVector.push_back("far");
        }

        // Relative orientation
        float relativeOrientation = playerKinematic.orientation - monsterKinematic.orientation;
        // Normalize to range [-180, 180]
        while (relativeOrientation > 180)
            relativeOrientation -= 360;
        while (relativeOrientation < -180)
            relativeOrientation += 360;

        if (std::abs(relativeOrientation) < 30.0f)
        {
            stateVector.push_back("direct_front");
        }
        else if (std::abs(relativeOrientation) < 90.0f)
        {
            stateVector.push_back("front");
        }
        else if (std::abs(relativeOrientation) < 150.0f)
        {
            stateVector.push_back("side");
        }
        else
        {
            stateVector.push_back("behind");
        }

        // Speed
        float speed = std::sqrt(monsterKinematic.velocity.x * monsterKinematic.velocity.x +
                                monsterKinematic.velocity.y * monsterKinematic.velocity.y);

        if (speed < 5.0f)
        {
            stateVector.push_back("stopped");
        }
        else if (speed < 50.0f)
        {
            stateVector.push_back("very_slow");
        }
        else if (speed < 100.0f)
        {
            stateVector.push_back("slow");
        }
        else if (speed < 150.0f)
        {
            stateVector.push_back("medium_speed");
        }
        else
        {
            stateVector.push_back("fast");
        }

        // Can see player
        bool canSeePlayer = monster.hasLineOfSightTo(playerKinematic.position);
        stateVector.push_back(canSeePlayer ? "1" : "0");

        // Is near obstacle
        bool isNearObstacle = false;
        bool isVeryNearObstacle = false;
        const Environment &env = monster.getEnvironment();

        // Check in several directions for obstacles
        for (int angle = 0; angle < 360; angle += 45)
        {
            float radian = angle * 3.14159f / 180.0f;
            float dx = std::cos(radian);
            float dy = std::sin(radian);

            sf::Vector2f checkPoint25(monsterKinematic.position.x + dx * 25.0f,
                                      monsterKinematic.position.y + dy * 25.0f);
            sf::Vector2f checkPoint50(monsterKinematic.position.x + dx * 50.0f,
                                      monsterKinematic.position.y + dy * 50.0f);

            if (env.isObstacle(checkPoint25))
            {
                isVeryNearObstacle = true;
                isNearObstacle = true;
                break;
            }
            else if (env.isObstacle(checkPoint50))
            {
                isNearObstacle = true;
            }
        }

        if (isVeryNearObstacle)
        {
            stateVector.push_back("very_near_obstacle");
        }
        else if (isNearObstacle)
        {
            stateVector.push_back("near_obstacle");
        }
        else
        {
            stateVector.push_back("no_obstacle");
        }

        // Path count
        int pathCount = monster.getPathCount();

        if (pathCount == 0)
        {
            stateVector.push_back("none");
        }
        else if (pathCount < 3)
        {
            stateVector.push_back("very_few");
        }
        else if (pathCount < 7)
        {
            stateVector.push_back("few");
        }
        else if (pathCount < 15)
        {
            stateVector.push_back("medium");
        }
        else
        {
            stateVector.push_back("many");
        }

        // Time in state
        float timeInAction = monster.getTimeInCurrentAction();

        if (timeInAction < 0.5f)
        {
            stateVector.push_back("very_short");
        }
        else if (timeInAction < 1.5f)
        {
            stateVector.push_back("short");
        }
        else if (timeInAction < 3.0f)
        {
            stateVector.push_back("medium");
        }
        else if (timeInAction < 5.0f)
        {
            stateVector.push_back("long");
        }
        else
        {
            stateVector.push_back("very_long");
        }

        // Add the current action as a feature for more context
        stateVector.push_back(monster.getCurrentAction());

        // Log the state vector for debugging
        std::cout << "State vector: ";
        for (const auto &attr : stateVector)
        {
            std::cout << attr << ", ";
        }
        std::cout << std::endl;

        return stateVector;
    }
};

#endif // LEARNED_DECISION_TREE_H