/**
 * @file PathFollower.h
 * @brief Defines the PathFollower class for following paths via steering behaviors.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - File: Provided Breadcrumb class from TA Derek Martin
 *
 * Author: Miles Hollifield
 * Date: 3/20/2025
 */

#ifndef PATHFOLLOWER_H
#define PATHFOLLOWER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <deque>
#include "headers/Arrive.h"
#include "headers/Align.h"
#include "headers/Kinematic.h"

/**
 * @class Breadcrumb
 * @brief Visual representation of past positions for tracking movement.
 */
class Breadcrumb
{
public:
    /**
     * @brief Constructor for a breadcrumb at the given position.
     * @param pos Position of the breadcrumb.
     */
    Breadcrumb(sf::Vector2f pos)
    {
        shape.setRadius(3.0f);
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(pos);
        shape.setOrigin(3.0f, 3.0f);
    }

    /**
     * @brief Draw the breadcrumb on the window.
     * @param window The window to draw to.
     */
    void draw(sf::RenderWindow &window) const
    {
        window.draw(shape);
    }

private:
    sf::CircleShape shape;
};

/**
 * @class PathFollower
 * @brief Agent that follows paths using steering behaviors.
 */
class PathFollower
{
public:
    /**
     * @brief Constructor for creating a path follower.
     * @param startPosition Initial position of the agent.
     * @param texture Texture for the agent sprite.
     */
    PathFollower(sf::Vector2f startPosition, sf::Texture &texture)
        : arriveBehavior(250.0f, 175.0f, 5.0f, 120.0f, 0.2f),
          alignBehavior(15.0f, 200.0f, 1.0f, 40.0f, 0.05f),
          breadcrumbCounter(0),
          currentWaypoint(0)
    {

        // Initialize kinematic state
        character.position = startPosition;
        character.velocity = {0, 0};
        character.orientation = 0;
        character.rotation = 0;

        // Initialize target
        target.position = startPosition;
        target.velocity = {0, 0};
        target.orientation = 0;
        target.rotation = 0;

        // Initialize sprite
        sprite.setTexture(texture);
        sprite.setScale(0.05f, 0.05f);
        sprite.setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);
        sprite.setPosition(startPosition);
    }

    /**
     * @brief Set a new path for the agent to follow.
     * @param waypoints Vector of points defining the path.
     */
    void setPath(const std::vector<sf::Vector2f> &waypoints)
    {
        if (waypoints.empty())
        {
            return;
        }

        path = waypoints;
        currentWaypoint = 0;

        // Set initial target to first waypoint
        if (!path.empty())
        {
            target.position = path[currentWaypoint];
        }

        // Clear breadcrumbs
        breadcrumbs.clear();
    }

    /**
     * @brief Set position of the agent directly.
     * @param position New position for the agent.
     */
    void setPosition(sf::Vector2f position)
    {
        character.position = position;
        character.velocity = {0, 0};
        sprite.setPosition(position);

        // Reset target and path
        target.position = position;
        path.clear();
        currentWaypoint = 0;

        // Clear breadcrumbs
        breadcrumbs.clear();
    }

    /**
     * @brief Update the agent's position to follow the path.
     * @param deltaTime Time since last update.
     */
    void update(float deltaTime)
    {
        // If no path or at the end of the path, don't move
        if (path.empty() || currentWaypoint >= path.size())
        {
            // Stop moving when we've reached the final waypoint
            character.velocity = {0, 0};
            character.rotation = 0;
            return;
        }

        // Target is the current waypoint we're moving toward
        target.position = path[currentWaypoint];

        // Calculate desired orientation based on direction to target
        sf::Vector2f dirToTarget = target.position - character.position;
        float distToTarget = std::sqrt(dirToTarget.x * dirToTarget.x + dirToTarget.y * dirToTarget.y);

        if (distToTarget > 0.1f)
        {
            // Normalize direction vector
            dirToTarget /= distToTarget;
            float desiredOrientation = std::atan2(dirToTarget.y, dirToTarget.x) * (180.0f / 3.14159265f);
            target.orientation = desiredOrientation;
        }

        // Apply Arrive behavior to navigate to current waypoint
        SteeringData arriveAcceleration = arriveBehavior.calculateAcceleration(character, target);
        character.velocity += arriveAcceleration.linear * deltaTime;

        // Limit velocity to max speed if needed
        float currentSpeed = std::sqrt(character.velocity.x * character.velocity.x +
                                       character.velocity.y * character.velocity.y);
        const float MAX_SPEED = 175.0f;
        if (currentSpeed > MAX_SPEED)
        {
            character.velocity *= (MAX_SPEED / currentSpeed);
        }

        // Apply Align behavior to match orientation
        SteeringData alignAcceleration = alignBehavior.calculateAcceleration(character, target);
        character.rotation += alignAcceleration.angular * deltaTime;

        if (character.velocity.x != 0 || character.velocity.y != 0)
        {
            float angle = std::atan2(character.velocity.y, character.velocity.x) * (180.0f / 3.14159265f);
            character.orientation = angle;
            sprite.setRotation(angle);
        }

        // Update character position and orientation
        character.update(deltaTime);

        // Check if we've reached the current waypoint
        distToTarget = std::sqrt(
            (target.position.x - character.position.x) * (target.position.x - character.position.x) +
            (target.position.y - character.position.y) * (target.position.y - character.position.y));

        // Waypoint reached threshold
        const float WAYPOINT_THRESHOLD = 10.0f;

        if (distToTarget < WAYPOINT_THRESHOLD)
        {
            std::cout << "Reached waypoint " << currentWaypoint << "/" << path.size() << std::endl;
            currentWaypoint++;
        }

        // Update sprite position and rotation
        sprite.setPosition(character.position);
        sprite.setRotation(character.orientation);

        // Handle breadcrumbs
        dropBreadcrumbs();
    }

    /**
     * @brief Check if the agent has reached the end of the path.
     * @return True if the path is completed.
     */
    bool pathCompleted() const
    {
        return path.empty() || currentWaypoint >= path.size();
    }

    /**
     * @brief Get the current position of the agent.
     * @return Current position.
     */
    sf::Vector2f getPosition() const
    {
        return character.position;
    }

    /**
     * @brief Draw the agent and its path on the window.
     * @param window The window to draw to.
     */
    void draw(sf::RenderWindow &window)
    {
        // Draw breadcrumbs first
        for (const auto &crumb : breadcrumbs)
        {
            crumb.draw(window);
        }

        // Draw the path
        if (path.size() > 1)
        {
            sf::VertexArray lines(sf::LinesStrip, path.size());
            for (size_t i = 0; i < path.size(); i++)
            {
                lines[i].position = path[i];
                lines[i].color = sf::Color(0, 150, 0, 150);
            }
            window.draw(lines);

            // Draw waypoints as small circles
            for (const auto &waypoint : path)
            {
                sf::CircleShape waypointMarker(5);
                waypointMarker.setFillColor(sf::Color(0, 100, 0));
                waypointMarker.setOrigin(5, 5);
                waypointMarker.setPosition(waypoint);
                window.draw(waypointMarker);
            }
        }

        // Draw the sprite
        window.draw(sprite);
    }

private:
    Kinematic character;   // The agent's kinematic state
    Kinematic target;      // The target's kinematic state
    sf::Sprite sprite;     // The agent's sprite
    Arrive arriveBehavior; // Arrive behavior for path following
    Align alignBehavior;   // Align behavior for orientation matching

    std::vector<sf::Vector2f> path; // The path to follow
    int currentWaypoint;            // Index of the current waypoint

    std::deque<Breadcrumb> breadcrumbs;            // Breadcrumbs for visualizing the path
    int breadcrumbCounter;                         // Counter for breadcrumb dropping
    static constexpr int BREADCRUMB_INTERVAL = 120; // Frames between dropping breadcrumbs
    static constexpr int MAX_BREADCRUMBS = 50;     // Maximum number of breadcrumbs

    /**
     * @brief Drop breadcrumbs to visualize the agent's path.
     */
    void dropBreadcrumbs()
    {
        breadcrumbCounter++;
        if (breadcrumbCounter >= BREADCRUMB_INTERVAL)
        {
            breadcrumbCounter = 0;
            breadcrumbs.push_back(Breadcrumb(character.position));

            if (breadcrumbs.size() > MAX_BREADCRUMBS)
            {
                breadcrumbs.pop_front();
            }
        }
    }
};

#endif // PATHFOLLOWER_H