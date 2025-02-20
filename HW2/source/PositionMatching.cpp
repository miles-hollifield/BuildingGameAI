#include "../headers/PositionMatching.h"
#include <cmath>

// Constructor
PositionMatching::PositionMatching(float maxAcceleration, float maxSpeed, float targetRadius, float slowRadius, float timeToTarget)
    : maxAcceleration(maxAcceleration), maxSpeed(maxSpeed), targetRadius(targetRadius), slowRadius(slowRadius), timeToTarget(timeToTarget) {}

SteeringData PositionMatching::calculateAcceleration(const Kinematic& character, const Kinematic& target) {
    SteeringData result;

    // Get direction to target
    sf::Vector2f direction = target.position - character.position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // If within the target radius, stop moving
    if (distance < targetRadius) {
        return SteeringData({0, 0}, 0);
    }

    // Determine target speed
    float targetSpeed = (distance > slowRadius) ? maxSpeed : maxSpeed * (distance / slowRadius);

    // Normalize direction and apply target speed
    sf::Vector2f targetVelocity = direction / distance * targetSpeed;

    // Compute acceleration needed
    result.linear = (targetVelocity - character.velocity) / timeToTarget;

    // Clamp acceleration if needed
    float accelMagnitude = std::sqrt(result.linear.x * result.linear.x + result.linear.y * result.linear.y);
    if (accelMagnitude > maxAcceleration) {
        result.linear = (result.linear / accelMagnitude) * maxAcceleration;
    }

    result.angular = 0;
    return result;
}
