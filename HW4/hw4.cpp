/**
 * @file hw4.cpp
 * @brief Main application for CSC 584/484 HW4 - Decision Trees, Behavior Trees, and Learning.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - Book: "Game AI Pro" edited by Steve Rabin
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

// Forward declarations
Environment createGameEnvironment(int width, int height);
std::shared_ptr<BehaviorTree> createMonsterBehaviorTree(Monster &monster);
std::shared_ptr<DecisionTree> createCharacterDecisionTree(EnvironmentState &state);
std::shared_ptr<DecisionTree> learnDecisionTreeFromBehaviorTree(const std::string &dataFile);
void recordBehaviorTreeData(Monster &monster, const std::string &outputFile, int frames);

/**
 * @brief Main function
 */
int main()
{
    // Create window
    int windowWidth = 800;
    int windowHeight = 600;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight),
                            "CSC 584/484 - HW4: Decision Trees and Behavior Trees");

    // Load textures
    sf::Texture playerTexture;
    sf::Texture monsterTexture;
    sf::Texture monsterLearnerTexture;

    if (!playerTexture.loadFromFile("player.png"))
    {
        std::cerr << "Failed to load player.png! Creating fallback texture." << std::endl;
        // Create a fallback texture
        playerTexture.create(32, 32);
        sf::Image img;
        img.create(32, 32, sf::Color::Green);
        playerTexture.update(img);
    }

    if (!monsterTexture.loadFromFile("monster.png"))
    {
        std::cerr << "Failed to load monster.png! Creating fallback texture." << std::endl;
        // Create a fallback texture
        monsterTexture.create(32, 32);
        sf::Image img;
        img.create(32, 32, sf::Color::Red);
        monsterTexture.update(img);
    }

    if (!monsterLearnerTexture.loadFromFile("monster_learner.png"))
    {
        std::cerr << "Failed to load monster_learner.png! Creating fallback texture." << std::endl;
        // Create a fallback texture
        monsterLearnerTexture.create(32, 32);
        sf::Image img;
        img.create(32, 32, sf::Color::Blue);
        monsterLearnerTexture.update(img);
    }

    // Create environment
    Environment environment = createGameEnvironment(windowWidth, windowHeight);

    // Create graph representation of the environment
    Graph environmentGraph = environment.createGraph(20); // 20px grid cells

    // Create player
    sf::Vector2f playerStartPos(100, 100);
    PathFollower player(playerStartPos, playerTexture);

    // Create behavior tree monster
    sf::Vector2f monsterStartPos(600, 500);
    Monster behaviorTreeMonster(monsterStartPos, monsterTexture, environment, environmentGraph);
    behaviorTreeMonster.setPlayerKinematic(player.getKinematic());
    behaviorTreeMonster.setControlType(Monster::ControlType::BEHAVIOR_TREE);

    // Create learned decision tree monster
    sf::Vector2f learnerStartPos(600, 100);
    Monster decisionTreeMonster(learnerStartPos, monsterLearnerTexture, environment, environmentGraph);
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
            "Left-click: Set player destination | R: Reset positions\n"
            "1: Record behavior tree data | 2: Learn decision tree | 3: Toggle monsters");
        instructionText.setCharacterSize(14);
        instructionText.setFillColor(sf::Color::Black);
        instructionText.setPosition(10, 10);
    }

    // Text for status
    sf::Text statusText;
    if (fontLoaded)
    {
        statusText.setFont(font);
        statusText.setCharacterSize(14);
        statusText.setFillColor(sf::Color::Black);
        statusText.setPosition(10, windowHeight - 30);
        statusText.setString("Click somewhere to move");
    }

    // Set up decision tree for the player
    EnvironmentState playerState(player.getKinematic());
    std::shared_ptr<DecisionTree> playerDecisionTree = createCharacterDecisionTree(playerState);

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
                        statusText.setString("Positions reset");
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
                            statusText.setString("Recording behavior tree data...");
                        }
                    }
                    else
                    {
                        isRecording = false;
                        recordingFile.close();

                        if (fontLoaded)
                        {
                            statusText.setString("Recording stopped");
                        }
                    }
                }
                else if (event.key.code == sf::Keyboard::Num2)
                {
                    // Learn decision tree from recorded data
                    std::shared_ptr<DecisionTree> learnedTree = learnDecisionTreeFromBehaviorTree(recordingFilename);
                    if (learnedTree)
                    {
                        decisionTreeMonster.setDecisionTree(learnedTree);
                        showDecisionTreeMonster = true;

                        if (fontLoaded)
                        {
                            statusText.setString("Decision tree learned and applied");
                        }
                    }
                    else
                    {
                        if (fontLoaded)
                        {
                            statusText.setString("Failed to learn decision tree");
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
                            statusText.setString("Showing learned decision tree monster");
                        }
                    }
                    else if (!showBehaviorTreeMonster && showDecisionTreeMonster)
                    {
                        showBehaviorTreeMonster = true;
                        showDecisionTreeMonster = true;
                        if (fontLoaded)
                        {
                            statusText.setString("Showing both monsters");
                        }
                    }
                    else
                    {
                        showBehaviorTreeMonster = true;
                        showDecisionTreeMonster = false;
                        if (fontLoaded)
                        {
                            statusText.setString("Showing behavior tree monster");
                        }
                    }
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    // Get mouse position
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                    // Check if the position is valid
                    if (!environment.isObstacle(mousePos))
                    {
                        // Find path to mouse position
                        int startVertex = environment.pointToVertex(player.getPosition());
                        int goalVertex = environment.pointToVertex(mousePos);

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
                            statusText.setString("Moving to new position");
                        }
                    }
                    else
                    {
                        if (fontLoaded)
                        {
                            statusText.setString("Cannot move to obstacle");
                        }
                    }
                }
            }
        }

        // Calculate delta time
        float deltaTime = gameClock.restart().asSeconds();

        // Update player
        player.update(deltaTime);

        // Update environment state for player
        playerState.update();

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
                    statusText.setString("Caught by behavior tree monster!");
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
                    statusText.setString("Caught by decision tree monster!");
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
                    statusText.setString("Recording complete (10000 frames)");
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
            performanceText.setPosition(windowWidth - 300, 10);

            std::string btStats = "Behavior Tree: " + std::to_string(behaviorTreeCatches) + " catches";
            std::string dtStats = "Decision Tree: " + std::to_string(decisionTreeCatches) + " catches";

            if (behaviorTreeCatches > 0)
            {
                btStats += " (avg " + std::to_string(behaviorTreeTime / behaviorTreeCatches) + " seconds)";
            }

            if (decisionTreeCatches > 0)
            {
                dtStats += " (avg " + std::to_string(decisionTreeTime / decisionTreeCatches) + " seconds)";
            }

            performanceText.setString(btStats + "\n" + dtStats);
            window.draw(performanceText);
        }

        // Draw text
        if (fontLoaded)
        {
            window.draw(instructionText);
            window.draw(statusText);
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
 * @brief Create the game environment with rooms and obstacles
 */
Environment createGameEnvironment(int width, int height)
{
    Environment env(width, height);

    // Add main room
    env.addRoom(sf::FloatRect(50, 50, width - 100, height - 100));

    // Add inner walls to create a maze-like structure

    // Horizontal walls
    env.addObstacle(sf::FloatRect(50, 200, 300, 20));
    env.addObstacle(sf::FloatRect(450, 200, 300, 20));

    env.addObstacle(sf::FloatRect(50, 400, 300, 20));
    env.addObstacle(sf::FloatRect(450, 400, 300, 20));

    // Vertical walls
    env.addObstacle(sf::FloatRect(200, 50, 20, 150));
    env.addObstacle(sf::FloatRect(200, 420, 20, 130));

    env.addObstacle(sf::FloatRect(400, 50, 20, 150));
    env.addObstacle(sf::FloatRect(400, 220, 20, 180));

    env.addObstacle(sf::FloatRect(600, 50, 20, 150));
    env.addObstacle(sf::FloatRect(600, 220, 20, 180));

    // Add obstacles
    env.addObstacle(sf::FloatRect(100, 100, 40, 40));
    env.addObstacle(sf::FloatRect(300, 150, 40, 40));
    env.addObstacle(sf::FloatRect(500, 100, 40, 40));

    env.addObstacle(sf::FloatRect(100, 300, 40, 40));
    env.addObstacle(sf::FloatRect(300, 300, 40, 40));
    env.addObstacle(sf::FloatRect(500, 300, 40, 40));

    env.addObstacle(sf::FloatRect(100, 500, 40, 40));
    env.addObstacle(sf::FloatRect(300, 450, 40, 40));
    env.addObstacle(sf::FloatRect(500, 500, 40, 40));

    return env;
}

/**
 * @brief Create a behavior tree for the monster
 */
std::shared_ptr<BehaviorTree> createMonsterBehaviorTree(Monster &monster)
{
    auto behaviorTree = std::make_shared<BehaviorTree>();

    // Create actions
    auto pathfindToPlayerAction = std::make_shared<BehaviorActionNode>(
        [&monster]() {
            monster.executeAction("PathfindToPlayer", 0.0f);
            return BehaviorStatus::SUCCESS;
        },
        "PathfindToPlayer"
    );

    auto wanderAction = std::make_shared<BehaviorActionNode>(
        [&monster]() {
            monster.executeAction("Wander", 0.0f);
            return BehaviorStatus::SUCCESS;
        },
        "Wander"
    );
    
    auto danceAction = std::make_shared<BehaviorActionNode>(
        [&monster]() {
            monster.executeAction("Dance", 0.0f);
            return BehaviorStatus::SUCCESS;
        },
        "Dance"
    );
    
    auto idleAction = std::make_shared<BehaviorActionNode>(
        [&monster]() {
            monster.executeAction("Idle", 0.0f);
            return BehaviorStatus::SUCCESS;
        },
        "Idle"
    );

    // Create conditions
    auto isNearPlayerCondition = std::make_shared<ConditionNode>(
        [&monster]() -> bool {
            // Check if monster is close to player
            const float NEAR_DISTANCE = 150.0f;
            return monster.hasCaughtPlayer() || 
                   (monster.getPosition() - monster.getPlayerKinematic().position).length() < NEAR_DISTANCE;
        },
        "IsNearPlayer"
    );

    auto canSeePlayerCondition = std::make_shared<ConditionNode>(
        [&monster]() -> bool {
            // Simple implementation: just check if player is in a 45-degree cone in front of monster
            sf::Vector2f toPlayer = monster.getPlayerKinematic().position - monster.getKinematic().position;
            float distance = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
            
            if (distance < 1.0f) return true; // Player is at same position
            
            // Normalize
            toPlayer /= distance;
            
            // Get forward vector of monster
            float radians = monster.getKinematic().orientation * 3.14159f / 180.0f;
            sf::Vector2f forward(std::cos(radians), std::sin(radians));
            
            // Dot product to find cos of angle between vectors
            float dot = forward.x * toPlayer.x + forward.y * toPlayer.y;
            
            // If dot > cos(45°), player is within 45° cone in front of monster
            return dot > 0.7071f; // cos(45°) = 0.7071
        },
        "CanSeePlayer"
    );
    
    auto hasDancedRecentlyCondition = std::make_shared<ConditionNode>(
        [&monster]() -> bool {
            // This condition would normally check a timer
            // For simplicity, we'll use a random chance (10%)
            return (rand() % 100) < 10;
        },
        "HasDancedRecently"
    );
    
    auto shouldDanceCondition = std::make_shared<ConditionNode>(
        [&monster]() -> bool {
            // Random chance to dance (5%)
            return (rand() % 100) < 5;
        },
        "ShouldDance"
    );
    
    // Now the composite nodes that use these conditions
    
    // Chase sequence: If we can see the player, pathfind to them
    auto chaseSequence = std::make_shared<SequenceNode>("Chase Sequence");
    chaseSequence->addChild(canSeePlayerCondition);
    chaseSequence->addChild(pathfindToPlayerAction);
    
    // Dance sequence: If we should dance and haven't recently, do a dance
    auto danceSequence = std::make_shared<SequenceNode>("Dance Sequence");
    danceSequence->addChild(shouldDanceCondition);
    danceSequence->addChild(std::make_shared<InverterNode>(hasDancedRecentlyCondition));
    danceSequence->addChild(danceAction);
    
    // Wander fallback: If nothing else to do, wander
    auto wanderFallback = std::make_shared<SelectorNode>("Wander Fallback");
    wanderFallback->addChild(danceSequence);
    wanderFallback->addChild(wanderAction);
    
    // Main selector
    auto rootSelector = std::make_shared<SelectorNode>("Root Selector");
    rootSelector->addChild(chaseSequence);
    rootSelector->addChild(wanderFallback);
    
    // Set the root node
    behaviorTree->setRootNode(rootSelector);

    return behaviorTree;
}

/**
 * @brief Create a decision tree for the player character
 */
std::shared_ptr<DecisionTree> createCharacterDecisionTree(EnvironmentState &state)
{
    auto decisionTree = std::make_shared<DecisionTree>(state);

    // Create potential target positions
    std::vector<sf::Vector2f> potentialTargets = {
        {100, 100}, {700, 100}, {400, 300}, {100, 500}, {700, 500}};

    // Build a sample decision tree
    decisionTree->buildSampleTree(potentialTargets);

    return decisionTree;
}

/**
 * @brief Learn a decision tree from recorded behavior tree data
 */
std::shared_ptr<DecisionTree> learnDecisionTreeFromBehaviorTree(const std::string &dataFile)
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

    // Learn decision tree
    std::shared_ptr<DTNode> dtRoot = learner.learnTree();

    // Print the learned tree
    std::cout << "Learned Decision Tree:" << std::endl;
    std::cout << learner.printTree() << std::endl;

    // Create a decision tree that uses the learned tree
    class LearnedDecisionTree : public DecisionTree
    {
    public:
        LearnedDecisionTree(std::shared_ptr<DTNode> root, EnvironmentState &state)
            : DecisionTree(state), dtRoot(root) {}

        std::string makeDecision() override
        {
            // Gather state variables from environment
            std::vector<std::string> stateVector = getStateVector();

            // Classify using the learned tree
            return dtRoot->classify(stateVector);
        }

    private:
        std::shared_ptr<DTNode> dtRoot;

        std::vector<std::string> getStateVector()
        {
            // Create a vector of state variables in the same format as the training data
            std::vector<std::string> state;

            // In a real implementation, these would be calculated based on the actual state
            // but for this example, we'll use placeholder values

            // Distance to player (discretized)
            state.push_back("medium"); // "near", "medium", "far"

            // Relative orientation (discretized)
            state.push_back("front"); // "front", "side", "back"

            // Speed (discretized)
            state.push_back("slow"); // "stopped", "slow", "fast"

            // Can see player
            state.push_back("1"); // "0" (false) or "1" (true)

            // Is near obstacle
            state.push_back("0"); // "0" (false) or "1" (true)

            // Path count (discretized)
            state.push_back("medium"); // "none", "few", "medium", "many"

            // Time in state (discretized)
            state.push_back("short"); // "short", "medium", "long"

            return state;
        }
    };

    // Create a simple environment state to pass to the decision tree
    Kinematic dummyKinematic;
    EnvironmentState dummyState(dummyKinematic);

    // Create the learned decision tree
    return std::make_shared<LearnedDecisionTree>(dtRoot, dummyState);
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
    sf::RenderWindow window(sf::VideoMode(800, 600), "Recording Data");

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