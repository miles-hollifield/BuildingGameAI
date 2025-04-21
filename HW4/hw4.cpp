/**
 * @file hw4.cpp
 * @brief Main application for CSC 584/484 HW4 - Decision Trees, Behavior Trees, and Learning.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - AI Tools: OpenAI's ChatGPT
 *
 * Author: Miles Hollifield
 * Date: 4/7/2025
 */

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <memory>

// Include headers from HW2 and HW3
#include "headers/Environment.h"
#include "headers/Graph.h"
#include "headers/Kinematic.h"
#include "headers/Arrive.h"
#include "headers/Align.h"
#include "headers/PathFollower.h"
#include "headers/Dijkstra.h"
#include "headers/AStar.h"

// Include headers for HW4
#include "headers/DecisionTree.h"
#include "headers/BehaviorTree.h"
#include "headers/Monster.h"
#include "headers/DTLearning.h"
#include "headers/LearnedDecisionTree.h"

// Forward declarations
Environment createIndoorEnvironment(int width, int height);
std::shared_ptr<BehaviorTree> createMonsterBehaviorTree(Monster &monster);
std::shared_ptr<DecisionTree> createCharacterDecisionTree(EnvironmentState &state, Environment &environment);
std::shared_ptr<DecisionTree> learnDecisionTreeFromBehaviorTree(const std::string &dataFile, Monster &monster);
void recordBehaviorTreeData(Monster &monster, const std::string &outputFile, int frames);

/**
 * @brief Creates an indoor environment with multiple rooms and obstacles.
 * @param width Width of the environment.
 * @param height Height of the environment.
 * @return Environment object with rooms and obstacles.
 */
Environment createIndoorEnvironment(int width, int height)
{
    Environment env(width, height);

    // Create a large background room that encompasses the entire space
    env.addRoom(sf::FloatRect(30, 30, 580, 420));

    // Top horizontal divider with doorway gap
    env.addObstacle(sf::FloatRect(30, 170, 225, 5));  // Left section
    env.addObstacle(sf::FloatRect(350, 170, 260, 5)); // Right section

    // Vertical divider with two doorway gaps
    env.addObstacle(sf::FloatRect(330, 30, 5, 70));  // Top section
    env.addObstacle(sf::FloatRect(330, 230, 5, 80)); // Middle section
    env.addObstacle(sf::FloatRect(330, 390, 5, 60)); // Bottom section

    // Left vertical divider with doorway
    env.addObstacle(sf::FloatRect(170, 170, 5, 90));  // Upper section
    env.addObstacle(sf::FloatRect(170, 350, 5, 100)); // Lower section

    // Right vertical divider with doorway
    env.addObstacle(sf::FloatRect(490, 170, 5, 90));  // Upper section
    env.addObstacle(sf::FloatRect(490, 350, 5, 100)); // Lower section

    // Bottom horizontal divider with doorway gaps
    env.addObstacle(sf::FloatRect(30, 330, 70, 5)); // Left section
    // Removed middle sections as you did
    env.addObstacle(sf::FloatRect(540, 330, 70, 5)); // Right section

    // Add obstacles within rooms

    // Top-right room obstacles
    // env.addObstacle(sf::FloatRect(410, 80, 20, 30));

    // Middle-left room obstacles
    env.addObstacle(sf::FloatRect(80, 240, 30, 20));

    // Middle-center room obstacles
    env.addObstacle(sf::FloatRect(260, 240, 20, 35));

    // Bottom row room obstacles
    env.addObstacle(sf::FloatRect(120, 380, 20, 20));
    env.addObstacle(sf::FloatRect(270, 380, 20, 25));
    env.addObstacle(sf::FloatRect(530, 380, 30, 20));

    return env;
}

/**
 * @brief Main function
 * @return Exit status
 * OpenAI's ChatGPT was used to assist in creating the main function structure.
 * The following prompt was used: "Create the main function for a C++ SFML application that
 * initializes a window, loads textures, creates an environment, and sets up agents with decision trees and behavior trees."
 * The code provided by ChatGPT was modified to fit the context of the project.
 */
int main()
{
    // Create window
    int windowWidth = 640;
    int windowHeight = 480;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight),
                            "CSC 584/484 - HW4: Decision Trees and Behavior Trees");

    // Load texture for the agents
    sf::Texture agentTexture;
    if (!agentTexture.loadFromFile("boid.png"))
    {
        std::cerr << "Failed to load boid.png! Creating fallback texture." << std::endl;
        // Create a fallback texture
        agentTexture.create(32, 32);
        sf::Image img;
        img.create(32, 32, sf::Color::Green);
        agentTexture.update(img);
    }

    // Create environment using the indoor environment from HW3
    Environment environment = createIndoorEnvironment(windowWidth, windowHeight);

    // Create graph representation of the environment
    Graph environmentGraph = environment.createGraph(20); // 20px grid cells

    // Create player
    sf::Vector2f playerStartPos(100, 100);
    PathFollower player(playerStartPos, agentTexture);

    // Create behavior tree monster
    sf::Vector2f monsterStartPos(400, 400);
    Monster behaviorTreeMonster(monsterStartPos, agentTexture, environment, environmentGraph, sf::Color::Red);
    behaviorTreeMonster.setPlayerKinematic(player.getKinematic());
    behaviorTreeMonster.setControlType(Monster::ControlType::BEHAVIOR_TREE);

    // Create learned decision tree monster
    sf::Vector2f learnerStartPos(450, 140);
    Monster decisionTreeMonster(learnerStartPos, agentTexture, environment, environmentGraph, sf::Color::Blue);
    decisionTreeMonster.setPlayerKinematic(player.getKinematic());
    decisionTreeMonster.setControlType(Monster::ControlType::DECISION_TREE);

    // Create behavior tree
    std::shared_ptr<BehaviorTree> behaviorTree = createMonsterBehaviorTree(behaviorTreeMonster);
    behaviorTreeMonster.setBehaviorTree(behaviorTree);

    // Create font for text display
    sf::Font font;
    bool fontLoaded = false;
    if (font.loadFromFile("ARIAL.TTF"))
    {
        fontLoaded = true;
    }

    // Text for instructions
    sf::Text instructionText;
    if (fontLoaded)
    {
        instructionText.setFont(font);
        instructionText.setString(
            "R: Reset positions |\n"
            "1: Record behavior tree data |\n"
            "2: Learn decision tree |\n"
            "3: Toggle monsters |");
        instructionText.setCharacterSize(14);
        instructionText.setFillColor(sf::Color::Black);

        sf::FloatRect textBounds = instructionText.getLocalBounds();
        instructionText.setPosition(350, 50);
    }

    // Text for status
    sf::Text playerStatusText;
    sf::Text recordStatusText;
    if (fontLoaded)
    {
        // Player status text (left side)
        playerStatusText.setFont(font);
        playerStatusText.setCharacterSize(14);
        playerStatusText.setFillColor(sf::Color::Black);
        playerStatusText.setPosition(20, windowHeight - 25);
        playerStatusText.setString("Player using decision tree");

        // Record status text (right side)
        recordStatusText.setFont(font);
        recordStatusText.setCharacterSize(14);
        recordStatusText.setFillColor(sf::Color::Black);
        recordStatusText.setPosition(windowWidth - 300, windowHeight - 25);
        recordStatusText.setString("Press 1 to start recording data");

        // Status background
        sf::RectangleShape statusBackground;
        statusBackground.setSize(sf::Vector2f(windowWidth - 20, 30));
        statusBackground.setPosition(10, windowHeight - 40);
        statusBackground.setFillColor(sf::Color(240, 240, 240, 200));
    }

    // Set up decision tree for the player
    EnvironmentState playerState(player.getKinematic(), environment);
    std::shared_ptr<DecisionTree> playerDecisionTree = createCharacterDecisionTree(playerState, environment);

    // Variables for controlling simulation
    bool showBehaviorTreeMonster = true;
    bool showDecisionTreeMonster = false;
    bool isRecording = false;
    int recordingFrames = 0;
    std::string recordingFilename = "behavior_data.csv";
    std::ofstream recordingFile;

    // Variables for performance comparison
    int behaviorTreeCatches = 0;
    int decisionTreeCatches = 0;
    float behaviorTreeTime = 0;
    float decisionTreeTime = 0;

    // Variables for player decision tree
    float playerDecisionTimer = 0.0f;
    const float DECISION_INTERVAL = 2.0f; // Make decisions every 2 seconds
    std::vector<sf::Vector2f> potentialTargets = {
        {100, 100}, // Top-left room
        {500, 100}, // Top-right room
        {100, 350}, // Bottom-left room
        {500, 350}, // Bottom-right room
        {250, 250}  // Center
    };

    // Clock for timing
    sf::Clock gameClock;
    sf::Clock catchTimerBT;
    sf::Clock catchTimerDT;

    // Main game loop
    while (window.isOpen())
    {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
                else if (event.key.code == sf::Keyboard::R)
                {
                    // Reset positions
                    player.setPosition(playerStartPos);
                    behaviorTreeMonster.reset();
                    decisionTreeMonster.reset();

                    // Reset behavior tree
                    behaviorTree->reset();

                    // Reset performance metrics
                    behaviorTreeCatches = 0;
                    decisionTreeCatches = 0;
                    behaviorTreeTime = 0;
                    decisionTreeTime = 0;
                    catchTimerBT.restart();
                    catchTimerDT.restart();

                    if (fontLoaded)
                    {
                        playerStatusText.setString("Positions reset");
                        recordStatusText.setString("Stats reset");
                    }
                }
                else if (event.key.code == sf::Keyboard::Num1)
                {
                    // Record behavior tree data
                    if (!isRecording)
                    {
                        isRecording = true;
                        recordingFrames = 0;
                        recordingFilename = "behavior_data.csv";
                        recordingFile.open(recordingFilename);

                        // Write header
                        recordingFile << "DistanceToPlayer,RelativeOrientation,Speed,"
                                      << "CanSeePlayer,IsNearObstacle,PathCount,"
                                      << "TimeInCurrentAction,Action" << std::endl;

                        if (fontLoaded)
                        {
                            recordStatusText.setString("Recording behavior tree data...");
                        }
                    }
                    else
                    {
                        isRecording = false;
                        recordingFile.close();

                        if (fontLoaded)
                        {
                            recordStatusText.setString("Recording stopped - " +
                                                       std::to_string(recordingFrames) +
                                                       " frames collected");
                        }
                    }
                }
                else if (event.key.code == sf::Keyboard::Num2)
                {
                    // Learn decision tree from recorded data
                    std::shared_ptr<DecisionTree> learnedTree = learnDecisionTreeFromBehaviorTree(recordingFilename, decisionTreeMonster);
                    if (learnedTree)
                    {
                        decisionTreeMonster.setDecisionTree(learnedTree);
                        showDecisionTreeMonster = true;

                        if (fontLoaded)
                        {
                            recordStatusText.setString("Decision tree learned and applied");
                        }
                    }
                    else
                    {
                        if (fontLoaded)
                        {
                            recordStatusText.setString("Failed to learn decision tree");
                        }
                    }
                }
                else if (event.key.code == sf::Keyboard::Num3)
                {
                    // Toggle monsters
                    if (showBehaviorTreeMonster && !showDecisionTreeMonster)
                    {
                        showBehaviorTreeMonster = false;
                        showDecisionTreeMonster = true;
                        if (fontLoaded)
                        {
                            recordStatusText.setString("Showing learned decision tree monster");
                        }
                    }
                    else if (!showBehaviorTreeMonster && showDecisionTreeMonster)
                    {
                        showBehaviorTreeMonster = true;
                        showDecisionTreeMonster = true;
                        if (fontLoaded)
                        {
                            recordStatusText.setString("Showing both monsters");
                        }
                    }
                    else
                    {
                        showBehaviorTreeMonster = true;
                        showDecisionTreeMonster = false;
                        if (fontLoaded)
                        {
                            recordStatusText.setString("Showing behavior tree monster");
                        }
                    }
                }
            }
        }

        // Calculate delta time
        float deltaTime = gameClock.restart().asSeconds();

        // Update player state
        playerState.update();

        // Update player decision timer
        playerDecisionTimer += deltaTime;

        // Make player decisions based on the decision tree
        if (playerDecisionTimer >= DECISION_INTERVAL || player.pathCompleted())
        {
            playerDecisionTimer = 0.0f;

            // Make a decision based on the decision tree
            std::string decision = playerDecisionTree->makeDecision();

            // Process the decision
            if (decision.find("PathfindTo") != std::string::npos)
            {
                // Extract target from decision
                size_t startPos = decision.find("_");
                if (startPos != std::string::npos)
                {
                    std::string targetStr = decision.substr(startPos + 1);
                    size_t separatorPos = targetStr.find("_");
                    if (separatorPos != std::string::npos)
                    {
                        float x = std::stof(targetStr.substr(0, separatorPos));
                        float y = std::stof(targetStr.substr(separatorPos + 1));
                        sf::Vector2f targetPos(x, y);

                        // Find path to target position
                        int startVertex = environment.pointToVertex(player.getPosition());
                        int goalVertex = environment.pointToVertex(targetPos);

                        // Use A* to find a path
                        AStar astar([](int current, int goal, const Graph &g)
                                    {
                            sf::Vector2f currentPos = g.getVertexPosition(current);
                            sf::Vector2f goalPos = g.getVertexPosition(goal);
                            float dx = goalPos.x - currentPos.x;
                            float dy = goalPos.y - currentPos.y;
                            return std::sqrt(dx * dx + dy * dy); });

                        std::vector<int> path = astar.findPath(environmentGraph, startVertex, goalVertex);

                        // Convert path to waypoints
                        std::vector<sf::Vector2f> waypoints;
                        for (int vertex : path)
                        {
                            waypoints.push_back(environmentGraph.getVertexPosition(vertex));
                        }

                        // Set the path for the player to follow
                        player.setPath(waypoints);

                        if (fontLoaded)
                        {
                            playerStatusText.setString("Player: Moving to position (" +
                                                       std::to_string(int(x)) + "," +
                                                       std::to_string(int(y)) + ")");
                        }
                    }
                }
            }
            else if (decision == "Wander")
            {
                // Generate a random position within environment bounds
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> distribX(50, windowWidth - 50);
                std::uniform_int_distribution<> distribY(50, windowHeight - 50);

                sf::Vector2f randomTarget;
                bool validTarget = false;

                // Try to find a valid random position
                for (int attempt = 0; attempt < 10 && !validTarget; attempt++)
                {
                    randomTarget.x = distribX(gen);
                    randomTarget.y = distribY(gen);
                    if (!environment.isObstacle(randomTarget))
                    {
                        validTarget = true;
                    }
                }

                if (validTarget)
                {
                    // Find path to random position
                    int startVertex = environment.pointToVertex(player.getPosition());
                    int goalVertex = environment.pointToVertex(randomTarget);

                    AStar astar([](int current, int goal, const Graph &g)
                                {
                        sf::Vector2f currentPos = g.getVertexPosition(current);
                        sf::Vector2f goalPos = g.getVertexPosition(goal);
                        float dx = goalPos.x - currentPos.x;
                        float dy = goalPos.y - currentPos.y;
                        return std::sqrt(dx * dx + dy * dy); });

                    std::vector<int> path = astar.findPath(environmentGraph, startVertex, goalVertex);

                    // Convert path to waypoints
                    std::vector<sf::Vector2f> waypoints;
                    for (int vertex : path)
                    {
                        waypoints.push_back(environmentGraph.getVertexPosition(vertex));
                    }

                    // Set the path for the player to follow
                    player.setPath(waypoints);

                    if (fontLoaded)
                    {
                        playerStatusText.setString("Player: Wandering to random location (" +
                                                   std::to_string(int(randomTarget.x)) + "," +
                                                   std::to_string(int(randomTarget.y)) + ")");
                    }
                }
            }
            else if (decision == "Flee")
            {
                // Determine flee direction (away from nearest obstacle)
                float nearestObstacleDistance = 1000.0f;
                sf::Vector2f fleeDirection(0, 0);

                // Check in 8 directions for obstacles
                for (int angle = 0; angle < 360; angle += 45)
                {
                    float radian = angle * 3.14159f / 180.0f;
                    float dx = std::cos(radian);
                    float dy = std::sin(radian);

                    // Check for obstacles
                    for (float dist = 10.0f; dist <= 50.0f; dist += 10.0f)
                    {
                        sf::Vector2f checkPoint = player.getPosition() + sf::Vector2f(dx * dist, dy * dist);
                        if (environment.isObstacle(checkPoint))
                        {
                            if (dist < nearestObstacleDistance)
                            {
                                nearestObstacleDistance = dist;
                                fleeDirection = sf::Vector2f(-dx, -dy); // Opposite direction
                            }
                            break;
                        }
                    }
                }

                if (nearestObstacleDistance < 1000.0f)
                {
                    // Scale the flee direction to get a reasonable distance
                    sf::Vector2f fleeTarget = player.getPosition() + fleeDirection * 100.0f;

                    // Ensure the flee target is within environment bounds
                    fleeTarget.x = std::max(50.0f, std::min(windowWidth - 50.0f, fleeTarget.x));
                    fleeTarget.y = std::max(50.0f, std::min(windowHeight - 50.0f, fleeTarget.y));

                    // Find path to flee target
                    int startVertex = environment.pointToVertex(player.getPosition());
                    int goalVertex = environment.pointToVertex(fleeTarget);

                    AStar astar([](int current, int goal, const Graph &g)
                                {
                        sf::Vector2f currentPos = g.getVertexPosition(current);
                        sf::Vector2f goalPos = g.getVertexPosition(goal);
                        float dx = goalPos.x - currentPos.x;
                        float dy = goalPos.y - currentPos.y;
                        return std::sqrt(dx * dx + dy * dy); });

                    std::vector<int> path = astar.findPath(environmentGraph, startVertex, goalVertex);

                    // Convert path to waypoints
                    std::vector<sf::Vector2f> waypoints;
                    for (int vertex : path)
                    {
                        waypoints.push_back(environmentGraph.getVertexPosition(vertex));
                    }

                    // Set the path for the player to follow
                    player.setPath(waypoints);

                    if (fontLoaded)
                    {
                        playerStatusText.setString("Player: Fleeing from obstacle at " +
                                                   std::to_string(int(nearestObstacleDistance)) +
                                                   " pixels away");
                    }
                }
            }
            else if (decision == "Dance")
            {
                player.setPath({}); // Clear current path

                if (fontLoaded)
                {
                    playerStatusText.setString("Player: Dancing");
                }
            }
            else
            {
                // Default to selecting a random target from the predefined list
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> distrib(0, potentialTargets.size() - 1);
                sf::Vector2f target = potentialTargets[distrib(gen)];

                // Find path to target
                int startVertex = environment.pointToVertex(player.getPosition());
                int goalVertex = environment.pointToVertex(target);

                AStar astar([](int current, int goal, const Graph &g)
                            {
                    sf::Vector2f currentPos = g.getVertexPosition(current);
                    sf::Vector2f goalPos = g.getVertexPosition(goal);
                    float dx = goalPos.x - currentPos.x;
                    float dy = goalPos.y - currentPos.y;
                    return std::sqrt(dx * dx + dy * dy); });

                std::vector<int> path = astar.findPath(environmentGraph, startVertex, goalVertex);

                // Convert path to waypoints
                std::vector<sf::Vector2f> waypoints;
                for (int vertex : path)
                {
                    waypoints.push_back(environmentGraph.getVertexPosition(vertex));
                }

                // Set the path for the player to follow
                player.setPath(waypoints);

                if (fontLoaded)
                {
                    playerStatusText.setString("Player: Moving to random target (" +
                                               std::to_string(int(target.x)) + "," +
                                               std::to_string(int(target.y)) + ")");
                }
            }
        }

        // Update player
        player.update(deltaTime);

        // Update monsters
        bool behaviorTreeCaught = false;
        bool decisionTreeCaught = false;

        if (showBehaviorTreeMonster)
        {
            behaviorTreeCaught = behaviorTreeMonster.update(deltaTime);

            if (behaviorTreeCaught)
            {
                behaviorTreeCatches++;
                behaviorTreeTime += catchTimerBT.restart().asSeconds();
                player.setPosition(playerStartPos);
                behaviorTreeMonster.reset();

                if (fontLoaded)
                {
                    playerStatusText.setString("Player caught by behavior tree monster!");
                }
            }
        }

        if (showDecisionTreeMonster)
        {
            decisionTreeCaught = decisionTreeMonster.update(deltaTime);

            if (decisionTreeCaught)
            {
                decisionTreeCatches++;
                decisionTreeTime += catchTimerDT.restart().asSeconds();
                player.setPosition(playerStartPos);
                decisionTreeMonster.reset();

                if (fontLoaded)
                {
                    playerStatusText.setString("Player caught by decision tree monster!");
                }
            }
        }

        // Record data if recording is active
        if (isRecording)
        {
            behaviorTreeMonster.recordStateAction(recordingFile);
            recordingFrames++;

            // Limit recording to avoid huge files
            if (recordingFrames > 10000)
            {
                isRecording = false;
                recordingFile.close();

                if (fontLoaded)
                {
                    recordStatusText.setString("Recording complete (10000 frames)");
                }
            }
        }

        // Render
        window.clear(sf::Color::White);

        // Draw environment
        environment.draw(window);

        // Draw player
        player.draw(window);

        // Draw monsters
        if (showBehaviorTreeMonster)
        {
            behaviorTreeMonster.draw(window);
        }

        if (showDecisionTreeMonster)
        {
            decisionTreeMonster.draw(window);
        }

        // Draw performance statistics
        sf::Text performanceText;
        if (fontLoaded)
        {
            performanceText.setFont(font);
            performanceText.setCharacterSize(12);
            performanceText.setFillColor(sf::Color::Black);

            std::string btStats = "Behavior Tree: " + std::to_string(behaviorTreeCatches) + " catches";
            std::string dtStats = "Decision Tree: " + std::to_string(decisionTreeCatches) + " catches";

            if (behaviorTreeCatches > 0)
            {
                // Format to 3 decimal places
                float avgTime = behaviorTreeTime / behaviorTreeCatches;
                char buffer[64];
                sprintf(buffer, "%.3f", avgTime);
                btStats += " (avg " + std::string(buffer) + " seconds)";
            }

            if (decisionTreeCatches > 0)
            {
                // Format to 3 decimal places
                float avgTime = decisionTreeTime / decisionTreeCatches;
                char buffer[64];
                sprintf(buffer, "%.3f", avgTime);
                dtStats += " (avg " + std::string(buffer) + " seconds)";
            }

            performanceText.setString(btStats + "\n" + dtStats);

            sf::FloatRect textBounds = performanceText.getLocalBounds();
            performanceText.setPosition(35, 50);

            window.draw(performanceText);
        }

        // Draw text
        if (fontLoaded)
        {
            window.draw(instructionText);
            window.draw(playerStatusText);
            window.draw(recordStatusText);
        }

        window.display();
    }

    // Close file if still open
    if (recordingFile.is_open())
    {
        recordingFile.close();
    }

    return 0;
}

/**
 * @brief Create a behavior tree for the monster
 * @param monster Reference to the monster
 * @return Shared pointer to the created behavior tree
 * OpenAI's ChatGPT was used to assist in implementing this function.
 * The following prompt was used:
 * "Create a behavior tree for a monster that includes actions like pathfinding to the player,
 * wandering, fleeing, and dancing. Use C++ and SFML for the implementation."
 * The code provided by ChatGPT was modified to fit the context of the project.
 */
std::shared_ptr<BehaviorTree> createMonsterBehaviorTree(Monster &monster)
{
    auto behaviorTree = std::make_shared<BehaviorTree>();

    // Create actions
    auto pathfindToPlayerAction = std::make_shared<BehaviorActionNode>(
        [&monster]()
        {
            monster.executeAction("PathfindToPlayer", monster.getDeltaTime());
            return BehaviorStatus::SUCCESS;
        },
        "PathfindToPlayer");

    // Create a FollowPath action node
    auto followPathAction = std::make_shared<BehaviorActionNode>(
        [&monster]()
        {
            monster.executeAction("FollowPath", monster.getDeltaTime());
            return BehaviorStatus::SUCCESS;
        },
        "FollowPath");

    auto wanderAction = std::make_shared<BehaviorActionNode>(
        [&monster]()
        {
            monster.executeAction("Wander", monster.getDeltaTime());
            return BehaviorStatus::SUCCESS;
        },
        "Wander");

    auto sharedDanceState = std::make_shared<BehaviorState>();

    auto cardinalDanceAction = std::make_shared<BehaviorActionNode>(
        [&monster, state = sharedDanceState]() mutable
        {
            // If we're not initialized, start the dance
            if (!state->initialized)
            {
                state->initialized = true;
                state->timer = 0.0f;
                monster.executeAction("Dance", monster.getDeltaTime());
                return BehaviorStatus::RUNNING;
            }

            // Update the dance timer with the correct delta time
            state->timer += monster.getDeltaTime();

            // Continue dance while in progress
            if (state->timer < 2.0f)
            {
                monster.executeAction("Dance", monster.getDeltaTime());
                return BehaviorStatus::RUNNING;
            }

            // Dance is complete
            state->initialized = false;
            state->timer = 0.0f;
            return BehaviorStatus::SUCCESS;
        },
        "CardinalDance");

    auto fleeAction = std::make_shared<BehaviorActionNode>(
        [&monster]()
        {
            monster.executeAction("Flee", monster.getDeltaTime());
            return BehaviorStatus::SUCCESS;
        },
        "Flee");

    // Create conditions
    auto canSeePlayerCondition = std::make_shared<ConditionNode>(
        [&monster, lastSeenTimer = std::make_shared<float>(0.0f),
         playerLastSeen = std::make_shared<bool>(false)]() -> bool
        {
            // Get direction to player
            sf::Vector2f monsterPos = monster.getPosition();
            sf::Vector2f playerPos = monster.getPlayerKinematic().position;
            sf::Vector2f toPlayer = playerPos - monsterPos;

            // Calculate distance
            float distance = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

            // Update last seen timer
            *lastSeenTimer += monster.getDeltaTime();

            // Can always "see" if extremely close
            if (distance < 30.0f)
            {
                if (!*playerLastSeen)
                {
                    *playerLastSeen = true;
                    *lastSeenTimer = 0.0f;
                }
                return true;
            }

            // If too far away, can't see
            if (distance > 250.0f)
            {
                *playerLastSeen = false;
                return false;
            }

            // Check if player is within view cone (140 degrees)
            float monsterAngle = monster.getKinematic().orientation * 3.14159f / 180.0f;
            sf::Vector2f monsterDir(std::cos(monsterAngle), std::sin(monsterAngle));

            // Normalize to player direction
            toPlayer = toPlayer / distance;

            // Dot product gives cosine of angle between vectors
            float dot = monsterDir.x * toPlayer.x + monsterDir.y * toPlayer.y;

            // Check if within 70° (cos(70°) ≈ 0.342)
            bool withinViewCone = (dot > 0.342f);

            // If not in view cone, can't see
            if (!withinViewCone)
            {
                *playerLastSeen = false;
                return false;
            }

            // Check line of sight
            bool lineOfSight = monster.hasLineOfSightTo(playerPos);

            if (!lineOfSight)
            {
                *playerLastSeen = false;
                return false;
            }

            // If we get here, player is visible
            if (!*playerLastSeen)
            {
                *playerLastSeen = true;
                *lastSeenTimer = 0.0f;
            }

            return true;
        },
        "CanSeePlayer");

    auto isNearObstacleCondition = std::make_shared<ConditionNode>(
        [&monster, detectionCount = std::make_shared<int>(0)]() -> bool
        {
            // Check for nearby obstacles using raycasting
            sf::Vector2f position = monster.getPosition();
            const Environment &env = monster.getEnvironment();

            // Check primarily in the direction of movement
            sf::Vector2f velocity = monster.getKinematic().velocity;
            float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);

            // If not moving, no need to flee
            if (speed < 5.0f)
            {
                *detectionCount = 0;
                return false;
            }

            // Normalize velocity to get direction
            sf::Vector2f moveDir = velocity / speed;

            // Check for obstacles directly ahead at close distances
            for (float dist = 5.0f; dist <= 20.0f; dist += 5.0f)
            {
                sf::Vector2f checkPoint = position + moveDir * dist;
                if (env.isObstacle(checkPoint))
                {
                    (*detectionCount)++;
                    // Only flee if we've detected obstacles multiple times in a row
                    return *detectionCount >= 2;
                }
            }

            // Check in a 45-degree cone around movement direction
            for (int angleOffset = -45; angleOffset <= 45; angleOffset += 15)
            {
                if (angleOffset == 0)
                    continue; // Already checked straight ahead

                float radians = std::atan2(moveDir.y, moveDir.x) + (angleOffset * 3.14159f / 180.0f);
                sf::Vector2f rayDir(std::cos(radians), std::sin(radians));

                for (float dist = 5.0f; dist <= 15.0f; dist += 5.0f)
                {
                    sf::Vector2f checkPoint = position + rayDir * dist;
                    if (env.isObstacle(checkPoint))
                    {
                        (*detectionCount)++;
                        return *detectionCount >= 2;
                    }
                }
            }

            // No obstacles detected
            *detectionCount = 0;
            return false;
        },
        "IsNearObstacle");

    auto shouldDanceCondition = std::make_shared<ConditionNode>(
        [&monster, lastDanceTime = std::make_shared<float>(0.0f),
         cooldownTime = std::make_shared<float>(10.0f)]() mutable -> bool
        {
            // Add elapsed time to the cooldown timer
            *lastDanceTime += monster.getDeltaTime();

            // Only allow dancing after cooldown period (10 seconds)
            if (*lastDanceTime >= *cooldownTime)
            {
                // Random chance to dance (5%)
                bool shouldDance = (rand() % 100) < 5;

                if (shouldDance)
                {
                    std::cout << "DANCE CONDITION: Cooldown complete, triggering dance" << std::endl;
                    // Reset cooldown timer if we decide to dance
                    *lastDanceTime = 0.0f;
                    return true;
                }
            }

            return false;
        },
        "ShouldDance");

    // Create chase sequence
    auto chaseSequence = std::make_shared<SequenceNode>("Chase Sequence");
    chaseSequence->addChild(canSeePlayerCondition);
    chaseSequence->addChild(pathfindToPlayerAction);
    chaseSequence->addChild(followPathAction);

    // Create dance sequence
    auto danceSequence = std::make_shared<SequenceNode>("Dance Sequence");
    danceSequence->addChild(shouldDanceCondition);
    danceSequence->addChild(cardinalDanceAction);

    // Create flee sequence
    auto fleeSequence = std::make_shared<SequenceNode>("Flee Sequence");
    fleeSequence->addChild(isNearObstacleCondition);
    fleeSequence->addChild(fleeAction);

    // Main selector - prioritize actions
    auto rootSelector = std::make_shared<SelectorNode>("Root Selector");
    rootSelector->addChild(fleeSequence);  // First priority: flee from obstacles
    rootSelector->addChild(chaseSequence); // Second priority: chase player if visible
    rootSelector->addChild(danceSequence); // Third priority: occasionally dance
    rootSelector->addChild(wanderAction);  // Last resort: wander around

    // Set the root node
    behaviorTree->setRootNode(rootSelector);

    // Log tree creation
    std::cout << "Created behavior tree for monster" << std::endl;

    return behaviorTree;
}

std::shared_ptr<EnvironmentState> Monster::createEnvironmentState()
{
    // Create a new environment state for this monster
    auto state = std::make_shared<EnvironmentState>(monsterKinematic, environment);

    // If we have a player kinematic, set it as the target for the state
    if (playerKinematic)
    {
        state->setTarget(playerKinematic->position);
    }

    // Update the state to reflect current conditions
    state->update();

    return state;
}

/**
 * @brief Create a decision tree for the player character
 * This function implements a more complex decision tree for the player's autonomous movement
 */
std::shared_ptr<DecisionTree> createCharacterDecisionTree(EnvironmentState &state, Environment &environment)
{
    auto decisionTree = std::make_shared<DecisionTree>(state);

    // Create potential target positions adjusted for environment size
    std::vector<sf::Vector2f> potentialTargets = {
        {100, 100}, // Top-left room
        {500, 100}, // Top-right room
        {100, 350}, // Bottom-left room
        {500, 350}, // Bottom-right room
        {250, 250}  // Center
    };

    // Define conditions for decision making
    auto isNearObstacle = [&state]() -> bool
    {
        return state.isNearObstacle(40.0f);
    };

    auto isMovingFast = [&state]() -> bool
    {
        return state.isMovingFast(120.0f);
    };

    auto isIdleTooLong = [&state]() -> bool
    {
        return state.isIdleForTooLong(3.0f);
    };

    auto shouldChangeTarget = [&state]() -> bool
    {
        return state.shouldChangeTarget();
    };

    auto isInTopLeftRoom = [&state]() -> bool
    {
        return state.isInRoom(0);
    };

    auto isInTopRightRoom = [&state]() -> bool
    {
        return state.isInRoom(1);
    };

    auto isInBottomLeftRoom = [&state]() -> bool
    {
        return state.isInRoom(2);
    };

    auto isInBottomRightRoom = [&state]() -> bool
    {
        return state.isInRoom(3);
    };

    auto shouldDance = [&state]() -> bool
    {
        // 2% chance to dance when we're deciding what to do
        return (rand() % 100) < 2;
    };

    // Create action nodes for different targets
    auto pathfindToTopLeft = std::make_shared<ActionNode>("PathfindTo_100_100");
    auto pathfindToTopRight = std::make_shared<ActionNode>("PathfindTo_500_100");
    auto pathfindToBottomLeft = std::make_shared<ActionNode>("PathfindTo_100_350");
    auto pathfindToBottomRight = std::make_shared<ActionNode>("PathfindTo_500_350");
    auto pathfindToCenter = std::make_shared<ActionNode>("PathfindTo_250_250");
    auto wanderAction = std::make_shared<ActionNode>("Wander");
    auto fleeAction = std::make_shared<ActionNode>("Flee");
    auto danceAction = std::make_shared<ActionNode>("Dance");

    // Create a random decision node for target selection
    auto randomTargetNode1 = std::make_shared<RandomDecisionNode>("Choose New Target 1");
    randomTargetNode1->addChild(pathfindToTopRight, 10.0f);
    randomTargetNode1->addChild(pathfindToBottomLeft, 10.0f);
    randomTargetNode1->addChild(pathfindToCenter, 5.0f);

    auto randomTargetNode2 = std::make_shared<RandomDecisionNode>("Choose New Target 2");
    randomTargetNode2->addChild(pathfindToTopLeft, 10.0f);
    randomTargetNode2->addChild(pathfindToBottomRight, 10.0f);
    randomTargetNode2->addChild(pathfindToCenter, 5.0f);

    auto randomTargetNode3 = std::make_shared<RandomDecisionNode>("Choose New Target 3");
    randomTargetNode3->addChild(pathfindToTopLeft, 10.0f);
    randomTargetNode3->addChild(pathfindToBottomRight, 10.0f);
    randomTargetNode3->addChild(pathfindToCenter, 5.0f);

    auto randomTargetNode4 = std::make_shared<RandomDecisionNode>("Choose New Target 4");
    randomTargetNode4->addChild(pathfindToTopRight, 10.0f);
    randomTargetNode4->addChild(pathfindToBottomLeft, 10.0f);
    randomTargetNode4->addChild(pathfindToCenter, 5.0f);

    // Target selection subtree based on current room
    auto targetSelectionNode = std::make_shared<DecisionBranch>(
        isInTopLeftRoom,
        std::make_shared<DecisionBranch>(
            shouldChangeTarget,
            randomTargetNode1,
            pathfindToTopLeft,
            "Should change target in top-left room?"),
        std::make_shared<DecisionBranch>(
            isInTopRightRoom,
            std::make_shared<DecisionBranch>(
                shouldChangeTarget,
                randomTargetNode2,
                pathfindToTopRight,
                "Should change target in top-right room?"),
            std::make_shared<DecisionBranch>(
                isInBottomLeftRoom,
                std::make_shared<DecisionBranch>(
                    shouldChangeTarget,
                    randomTargetNode3,
                    pathfindToBottomLeft,
                    "Should change target in bottom-left room?"),
                std::make_shared<DecisionBranch>(
                    isInBottomRightRoom,
                    std::make_shared<DecisionBranch>(
                        shouldChangeTarget,
                        randomTargetNode4,
                        pathfindToBottomRight,
                        "Should change target in bottom-right room?"),
                    pathfindToCenter, // Default if not in any specific room
                    "In bottom-right room?"),
                "In bottom-left room?"),
            "In top-right room?"),
        "In top-left room?");

    // Special behavior branch
    auto specialBehaviorNode = std::make_shared<DecisionBranch>(
        shouldDance,
        danceAction,
        targetSelectionNode,
        "Should dance?");

    // Movement safety branch
    auto safetyNode = std::make_shared<DecisionBranch>(
        isNearObstacle,
        fleeAction,
        specialBehaviorNode,
        "Near obstacle?");

    // Main decision branch
    auto rootNode = std::make_shared<DecisionBranch>(
        isMovingFast,
        safetyNode,
        std::make_shared<DecisionBranch>(
            isIdleTooLong,
            wanderAction,
            targetSelectionNode,
            "Idle too long?"),
        "Moving fast?");

    // Set the root node
    decisionTree->setRootNode(rootNode);

    // Log tree creation
    std::cout << "Created decision tree for character" << std::endl;

    return decisionTree;
}

/**
 * @brief Learn a decision tree from recorded behavior tree data
 * @param dataFile Path to the recorded data file
 * @param monster Reference to the monster that will use the learned tree
 * @return Shared pointer to the learned decision tree
 */
std::shared_ptr<DecisionTree> learnDecisionTreeFromBehaviorTree(const std::string &dataFile, Monster &monster)
{
    // Create decision tree learner
    DecisionTreeLearner learner;

    // Set attribute names for better readability
    std::vector<std::string> attributeNames = {
        "DistanceToPlayer", "RelativeOrientation", "Speed",
        "CanSeePlayer", "IsNearObstacle", "PathCount", "TimeInState"};
    learner.setAttributeNames(attributeNames);

    // Load data from file
    if (!learner.loadData(dataFile))
    {
        std::cerr << "Failed to load data from " << dataFile << std::endl;
        return nullptr;
    }

    std::cout << "Loaded " << dataFile << " for learning" << std::endl;

    // Learn decision tree
    std::shared_ptr<DTNode> dtRoot = learner.learnTree();
    if (!dtRoot)
    {
        std::cerr << "Failed to learn decision tree" << std::endl;
        return nullptr;
    }

    std::cout << "Successfully learned decision tree" << std::endl;

    // Print the learned tree
    std::cout << "\nLEARNED DECISION TREE STRUCTURE:\n";
    std::cout << "--------------------------------\n";
    std::cout << learner.printTree() << "\n";
    std::cout << "--------------------------------\n";

    // Save tree to a file for reference
    learner.saveTree("learned_decision_tree.txt");
    std::cout << "Saved tree structure to learned_decision_tree.txt" << std::endl;

    // Create a decision tree that uses the learned tree
    std::cout << "Creating LearnedDecisionTree instance" << std::endl;
    return std::make_shared<LearnedDecisionTree>(dtRoot, monster);
}

/**
 * @brief Record data from the behavior tree monster for training
 */
void recordBehaviorTreeData(Monster &monster, const std::string &outputFile, int frames)
{
    std::ofstream file(outputFile);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file for recording: " << outputFile << std::endl;
        return;
    }

    // Write header
    file << "DistanceToPlayer,RelativeOrientation,Speed,"
         << "CanSeePlayer,IsNearObstacle,PathCount,"
         << "TimeInCurrentAction,Action" << std::endl;

    // Create window for simulation
    sf::RenderWindow window(sf::VideoMode(640, 480), "Recording Data");

    // Clock for delta time
    sf::Clock clock;

    // Record for specified number of frames
    for (int i = 0; i < frames && window.isOpen(); i++)
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // Update monster
        float deltaTime = clock.restart().asSeconds();
        monster.update(deltaTime);

        // Record state and action
        monster.recordStateAction(file);

        // Render (optional, can be disabled for faster recording)
        window.clear(sf::Color::White);
        monster.draw(window);
        window.display();
    }

    file.close();
}