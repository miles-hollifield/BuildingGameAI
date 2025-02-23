/**
 * @file WanderBoid.h
 * @brief Defines the WanderBoid class for implementing smooth wandering behavior with visual breadcrumbs.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - File: Provided Breadcrumb class from TA Derek Martin
 *
 * Author: Miles Hollifield
 * Date: 2/23/2025
 */

#ifndef WANDERBOID_H
#define WANDERBOID_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

// Constants for the boid's wander behavior
constexpr float MAX_SPEED = 150.0f; // Maximum speed of the boid
constexpr float WANDER_CIRCLE_DISTANCE = 50.0f; // Distance from the boid to the center of the wander circle
constexpr float WANDER_CIRCLE_RADIUS = 20.0f; // Radius of the wander circle
constexpr float WANDER_ANGLE_SMOOTHING = 3.0f; // Smoothing factor for the wander angle
constexpr float ROTATION_SMOOTH_FACTOR = 0.05f; // Smoothing factor for rotation
constexpr int BREADCRUMB_LIMIT = 30; // Maximum number of breadcrumbs
constexpr int BREADCRUMB_INTERVAL = 60; // Interval for dropping breadcrumbs
constexpr float WINDOW_WIDTH = 640; // Window width
constexpr float WINDOW_HEIGHT = 480; // Window height

/**
 * @class Crumb
 * @brief Represents a visual breadcrumb marking the WanderBoid's movement path.
 */
class Crumb {
public:
    /**
     * @brief Constructor to initialize breadcrumb position.
     * @param pos The position of the breadcrumb.
     */
    Crumb(sf::Vector2f pos);

    /**
     * @brief Draws the breadcrumb on the window.
     * @param window Reference to the SFML window.
     */
    void draw(sf::RenderWindow& window) const;
private:
    sf::CircleShape shape; // Shape of the breadcrumb
};

/**
 * @class WanderBoid
 * @brief Implements a boid that exhibits smooth wandering behavior.
 */
class WanderBoid {
public:
    /**
     * @brief Constructor to initialize a WanderBoid.
     * @param w Pointer to the SFML window.
     * @param tex Reference to the boid texture.
     * @param crumbs Pointer to a vector of breadcrumbs.
     */
    WanderBoid(sf::RenderWindow* w, sf::Texture& tex, std::vector<Crumb>* crumbs);

    /**
     * @brief Updates the boid's position, orientation, and breadcrumbs.
     * @param deltaTime Time since the last update.
     */
    void update(float deltaTime);

    /**
     * @brief Draws the boid on the window.
     */
    void draw();

private:
    sf::RenderWindow* window; // Pointer to the SFML window
    sf::Sprite sprite; // Sprite representing the boid
    sf::Vector2f position; // Current position of the boid
    sf::Vector2f velocity; // Current velocity of the boid
    float wanderAngle; // Current angle for wandering
    float orientation; // Current orientation of the boid
    std::vector<Crumb>* breadcrumbs; // Pointer to the vector of breadcrumbs

    std::random_device rd; // Random device for seeding
    std::mt19937 rng; // Random number generator
    std::uniform_real_distribution<float> angleChangeDist; // Distribution for angle changes

    /**
     * @brief Applies the wandering behavior to the boid.
     * @param deltaTime Time since the last update.
     */
    void applyWander(float deltaTime);

    /**
     * @brief Normalizes a vector to unit length.
     * @param vec The vector to normalize.
     * @return The normalized vector.
     */
    sf::Vector2f normalize(sf::Vector2f vec);

    /**
     * @brief Handles the boid's boundaries to keep it within the window.
     */
    void handleBoundaries();

    /**
     * @brief Drops breadcrumbs at the boid's current position.
     */
    void dropBreadcrumbs();

    /**
     * @brief Updates the boid's orientation based on its velocity.
     */
    void updateOrientation();
};

#endif // WANDERBOID_H
