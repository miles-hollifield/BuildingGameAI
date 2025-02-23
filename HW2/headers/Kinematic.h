/**
 * @file Kinematic.h
 * @brief Defines the Kinematic structure for position, velocity, orientation, and rotation.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 *
 * Author: Miles Hollifield
 * Date: 2/23/2025
 */

#ifndef KINEMATIC_H
#define KINEMATIC_H

#include <SFML/System.hpp>
#include <cmath>  // For fmod to normalize orientation

/**
 * @struct Kinematic
 * @brief Represents the kinematic state of an entity in the game world.
 */
struct Kinematic {
    sf::Vector2f position; // Position in 2D space
    sf::Vector2f velocity; // Velocity vector
    float orientation; // Orientation in degrees
    float rotation; // Rotation in degrees

    /**
     * @brief Default constructor to initialize kinematic state.
     * @param pos Initial position of the entity.
     * @param vel Initial velocity of the entity.
     * @param orient Initial orientation of the entity.
     * @param rot Initial angular velocity of the entity.
     */
    Kinematic(sf::Vector2f pos = {0, 0}, sf::Vector2f vel = {0, 0}, float orient = 0, float rot = 0)
        : position(pos), velocity(vel), orientation(orient), rotation(rot) {}

    /**
     * @brief Updates the kinematic state of the entity based on elapsed time.
     * @param deltaTime The time that has passed since the last update.
     */
    void update(float deltaTime) {
        // Update position using velocity
        position += velocity * deltaTime;
        // Update orientation using rotation
        orientation += rotation * deltaTime;

        // Normalize orientation to keep it within 0-360 degrees
        orientation = fmod(orientation, 360.0f);
        if (orientation < 0) orientation += 360.0f;
    }
};

#endif // KINEMATIC_H
