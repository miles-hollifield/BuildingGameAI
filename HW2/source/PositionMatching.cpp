#include "../headers/PositionMatching.h"
#include <cmath>

SteeringData PositionMatching::calculateAcceleration(const Kinematic& character, const Kinematic& target) {
    SteeringData result;

    // Compute the direction to the target
    sf::Vector2f direction = target.position - character.position;

    // Normalize the direction vector
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (distance > 0) {
        direction /= distance;  // Normalize
    }

    // Apply max acceleration in that direction
    result.linear = direction * maxAcceleration;

    result.angular = 0;  // No angular acceleration in position matching
    return result;
}
