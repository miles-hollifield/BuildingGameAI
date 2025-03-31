/**
 * @file Environment.h
 * @brief Defines the Environment class for creating indoor environments.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - AI Tools: OpenAI's ChatGPT
 *
 * Author: Miles Hollifield
 * Date: 3/20/2025
 */

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "Graph.h"

/**
 * @class Environment
 * @brief Represents an indoor environment with rooms and obstacles.
 */
class Environment
{
public:
    /**
     * @brief Constructor for creating an environment with specified dimensions.
     * @param environmentWidth Width of the environment.
     * @param environmentHeight Height of the environment.
     */
    Environment(int environmentWidth, int environmentHeight)
        : environmentWidth(environmentWidth), environmentHeight(environmentHeight) {}

    /**
     * @brief Add a room to the environment.
     * @param room Rectangle representing the room.
     */
    void addRoom(const sf::FloatRect &room)
    {
        rooms.push_back(room);
    }

    /**
     * @brief Add an obstacle to the environment.
     * @param obstacle Rectangle representing the obstacle.
     */
    void addObstacle(const sf::FloatRect &obstacle)
    {
        obstacles.push_back(obstacle);
    }

    /**
     * @brief Create a graph representation of the environment.
     * @param gridSize Size of the grid cells.
     * @return Graph representing the environment.
     * @note OpenAI's ChatGPT was used here to suggest a method for creating a graph representation of the environment.
     * The following prompt was used: "How can I create a graph representation of an indoor environment with rooms and obstacles in C++?
     * Suggest an algorithm or method to do this efficiently."
     * The response was modified to fit the context of the code.
     */
    Graph createGraph(int gridSize)
    {
        // Calculate number of vertices in grid
        int gridColumns = environmentWidth / gridSize;
        int gridRows = environmentHeight / gridSize;
        int numVertices = gridColumns * gridRows;

        // Create graph
        Graph graph(numVertices);

        // Create vertex positions
        std::vector<sf::Vector2f> positions;
        vertexPositions.clear();

        for (int row = 0; row < gridRows; row++)
        {
            for (int col = 0; col < gridColumns; col++)
            {
                int vertexIndex = row * gridColumns + col;
                sf::Vector2f position((col + 0.5f) * gridSize, (row + 0.5f) * gridSize);
                positions.push_back(position);
                vertexPositions.push_back(position);
            }
        }

        // Store positions in graph
        graph.setVertexPositions(positions);

        // Define direction arrays for 8-connected grid
        const int directionX[] = {-1, 0, 1, 0, -1, -1, 1, 1};
        const int directionY[] = {0, -1, 0, 1, -1, 1, -1, 1};

        for (int row = 0; row < gridRows; row++)
        {
            for (int col = 0; col < gridColumns; col++)
            {
                int vertexIndex = row * gridColumns + col;
                sf::Vector2f position = positions[vertexIndex];

                // Skip if current position is inside an obstacle
                if (isObstacle(position))
                {
                    continue;
                }

                // Connect to adjacent vertices
                for (int direction = 0; direction < 8; direction++)
                {
                    int neighborRow = row + directionY[direction];
                    int neighborCol = col + directionX[direction];

                    // Check bounds
                    if (neighborRow < 0 || neighborRow >= gridRows || neighborCol < 0 || neighborCol >= gridColumns)
                    {
                        continue;
                    }

                    int neighborIndex = neighborRow * gridColumns + neighborCol;
                    sf::Vector2f neighborPosition = positions[neighborIndex];

                    // Skip if neighbor is in obstacle
                    if (isObstacle(neighborPosition))
                    {
                        continue;
                    }

                    // Check if there's a clear path between vertices (no obstacles in the way)
                    if (hasLineOfSight(position, neighborPosition))
                    {
                        // Calculate Euclidean distance as edge weight
                        float distanceX = neighborPosition.x - position.x;
                        float distanceY = neighborPosition.y - position.y;
                        float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

                        // Add edge with weight
                        graph.addEdge(vertexIndex, neighborIndex, distance);
                    }
                }
            }
        }

        gridColumns = gridColumns; // Store for conversion functions
        return graph;
    }

    /**
     * @brief Check if there's a clear line of sight between two points.
     * @param from Start point.
     * @param to End point.
     * @return True if there's a clear line of sight.
     * @note OpenAI's ChatGPT was used here to suggest a method for checking line of sight between two points.
     * The following prompt was used: "How can I check if there's a clear line of sight between two points in a 2D grid?
     * Suggest an algorithm or method to do this efficiently."
     * The response was modified to fit the context of the code.
     */
    bool hasLineOfSight(const sf::Vector2f &from, const sf::Vector2f &to)
    {
        // Check if the line between from and to intersects with any obstacles
        float distanceX = std::abs(to.x - from.x);
        float distanceY = std::abs(to.y - from.y);

        int x = static_cast<int>(from.x);
        int y = static_cast<int>(from.y);

        int stepCount = 1 + static_cast<int>(distanceX + distanceY);
        int xIncrement = (to.x > from.x) ? 1 : -1;
        int yIncrement = (to.y > from.y) ? 1 : -1;

        float error = distanceX - distanceY;
        distanceX *= 2;
        distanceY *= 2;

        // Check if there's an obstacle along the line
        for (; stepCount > 0; --stepCount)
        {
            sf::Vector2f checkPoint(static_cast<float>(x), static_cast<float>(y));

            if (isObstacle(checkPoint))
            {
                return false; // Obstacle in the way
            }

            if (error > 0)
            {
                x += xIncrement;
                error -= distanceY;
            }
            else
            {
                y += yIncrement;
                error += distanceX;
            }
        }

        return true; // No obstacles in the way
    }

    /**
     * @brief Convert a point in the environment to a vertex in the graph.
     * @param point The point to convert.
     * @return Vertex index in the graph.
     */
    int pointToVertex(const sf::Vector2f &point) const
    {
        // Find the closest vertex to the given point
        int bestVertex = -1;
        float bestDistance = std::numeric_limits<float>::max();

        // Iterate through all vertices to find the closest one
        for (int i = 0; i < vertexPositions.size(); i++)
        {
            float distanceX = point.x - vertexPositions[i].x;
            float distanceY = point.y - vertexPositions[i].y;
            float squaredDistance = distanceX * distanceX + distanceY * distanceY;

            if (squaredDistance < bestDistance)
            {
                bestDistance = squaredDistance;
                bestVertex = i;
            }
        }

        return bestVertex;
    }

    /**
     * @brief Convert a vertex in the graph to a point in the environment.
     * @param vertex Vertex index in the graph.
     * @return Point in the environment.
     */
    sf::Vector2f vertexToPoint(int vertex) const
    {
        if (vertex >= 0 && vertex < vertexPositions.size())
        {
            return vertexPositions[vertex];
        }
        return {0, 0}; // Invalid vertex
    }

    /**
     * @brief Check if a point is inside an obstacle.
     * @param point The point to check.
     * @return True if the point is inside an obstacle.
     */
    bool isObstacle(const sf::Vector2f &point) const
    {
        // Check if point is outside environment bounds
        if (point.x < 0 || point.x >= environmentWidth || point.y < 0 || point.y >= environmentHeight)
        {
            return true;
        }

        // Check if point is inside any obstacle
        for (const auto &obstacle : obstacles)
        {
            if (obstacle.contains(point))
            {
                return true;
            }
        }

        // Check if point is outside all rooms
        bool insideAnyRoom = false;
        for (const auto &room : rooms)
        {
            if (room.contains(point))
            {
                insideAnyRoom = true;
                break;
            }
        }

        return !insideAnyRoom; // If not inside any room, it's considered an obstacle
    }

    /**
     * @brief Draw the environment to a window.
     * @param window The window to draw to.
     */
    void draw(sf::RenderWindow &window) const
    {
        // Draw rooms
        for (const auto &room : rooms)
        {
            sf::RectangleShape roomShape({room.width, room.height});
            roomShape.setPosition({room.left, room.top});
            roomShape.setFillColor(sf::Color(240, 240, 240));
            roomShape.setOutlineColor(sf::Color::Black);
            roomShape.setOutlineThickness(2);
            window.draw(roomShape);
        }

        // Draw obstacles
        for (const auto &obstacle : obstacles)
        {
            sf::RectangleShape obstacleShape({obstacle.width, obstacle.height});
            obstacleShape.setPosition({obstacle.left, obstacle.top});
            obstacleShape.setFillColor(sf::Color(100, 100, 100));
            window.draw(obstacleShape);
        }
    }

private:
    int environmentWidth, environmentHeight;   // Dimensions of the environment
    std::vector<sf::FloatRect> rooms;          // List of rooms in the environment
    std::vector<sf::FloatRect> obstacles;      // List of obstacles in the environment
    std::vector<sf::Vector2f> vertexPositions; // Positions of vertices in the graph
    int gridColumns;                           // Number of columns in the grid
};

#endif // ENVIRONMENT_H