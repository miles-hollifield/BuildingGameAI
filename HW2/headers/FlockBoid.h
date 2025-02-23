/**
 * @file FlockBoid.h
 * @brief Defines the FlockBoid class for implementing flocking behavior (separation, alignment, and cohesion).
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 *
 * Author: Miles Hollifield
 * Date: 2/23/2025
 */

#ifndef FLOCKBOID_H
#define FLOCKBOID_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

/**
 * @class FlockBoid
 * @brief Implements a boid that exhibits flocking behavior using separation, alignment, and cohesion.
 */
class FlockBoid {
public:
    /**
     * @brief Constructor to initialize a FlockBoid.
     * @param x Initial x-coordinate.
     * @param y Initial y-coordinate.
     * @param texture Reference to the boid texture.
     */
    FlockBoid(float x, float y, sf::Texture& texture);

    /**
     * @brief Updates the boid's behavior based on flocking rules.
     * @param deltaTime Time since the last update.
     * @param flock Reference to the vector of all flock members.
     */
    void update(float deltaTime, const std::vector<FlockBoid>& flock);

    /**
     * @brief Draws the boid on the window.
     * @param window Reference to the SFML window.
     */
    void draw(sf::RenderWindow& window);
    
    /**
     * @brief Gets the current position of the boid.
     * @return The current position.
     */
    sf::Vector2f getPosition() const { return position; }

    /**
     * @brief Gets the current velocity of the boid.
     * @return The current velocity.
     */
    sf::Vector2f getVelocity() const { return velocity; }

private:
    sf::Vector2f position; // Current position of the boid
    sf::Vector2f velocity; // Current velocity of the boid
    sf::Vector2f acceleration; // Current acceleration of the boid
    sf::Sprite sprite; // Sprite representing the boid

    static constexpr float MAX_SPEED = 100.0f; // Maximum speed of the boid
    static constexpr float MAX_FORCE = 5.0f; // Maximum steering force
    static constexpr float SEPARATION_RADIUS = 25.0f; // Radius for separation
    static constexpr float ALIGNMENT_RADIUS = 50.0f; // Radius for alignment
    static constexpr float COHESION_RADIUS = 50.0f; // Radius for cohesion

    /**
     * @brief Calculates the steering force for separation from nearby boids.
     * @param flock Reference to the vector of all flock members.
     */
    sf::Vector2f separate(const std::vector<FlockBoid>& flock);

    /**
     * @brief Calculates the steering force for alignment with nearby boids.
     * @param flock Reference to the vector of all flock members.
     */
    sf::Vector2f align(const std::vector<FlockBoid>& flock);

    /**
     * @brief Calculates the steering force for cohesion towards nearby boids.
     * @param flock Reference to the vector of all flock members.
     */
    sf::Vector2f cohere(const std::vector<FlockBoid>& flock);

    /**
     * @brief Limits the magnitude of a vector to a maximum value.
     * @param vec The vector to limit.
     * @param max The maximum value.
     * @return The limited vector.
     */
    sf::Vector2f limit(sf::Vector2f vec, float max);

    /**
     * @brief Normalizes a vector to a unit vector.
     * @param vec The vector to normalize.
     * @return The normalized vector.
     */
    sf::Vector2f normalize(sf::Vector2f vec);
};

#endif // FLOCKBOID_H
