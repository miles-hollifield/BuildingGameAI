#ifndef KINEMATIC_H
#define KINEMATIC_H

#include <SFML/System.hpp>
#include <cmath>  // For fmod (to normalize orientation)

struct Kinematic {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float orientation;
    float rotation;

    // Default Constructor
    Kinematic(sf::Vector2f pos = {0, 0}, sf::Vector2f vel = {0, 0}, float orient = 0, float rot = 0)
        : position(pos), velocity(vel), orientation(orient), rotation(rot) {}

    // Function to update kinematic state using deltaTime
    void update(float deltaTime) {
        position += velocity * deltaTime;
        orientation += rotation * deltaTime;

        // Normalize orientation to keep it within 0-360 degrees
        orientation = fmod(orientation, 360.0f);
        if (orientation < 0) orientation += 360.0f;
    }
};

#endif // KINEMATIC_H
