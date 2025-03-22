#include "../headers/Arrive.h"
#include <cmath>

// Constructor
Arrive::Arrive(float maxAcceleration, float maxSpeed, float targetRadius, float slowRadius, float timeToTarget)
    : maxAcceleration(maxAcceleration), maxSpeed(maxSpeed), 
      targetRadius(targetRadius), slowRadius(slowRadius), timeToTarget(timeToTarget) {}

SteeringData Arrive::calculateAcceleration(const Kinematic& character, const Kinematic& target) {
    SteeringData result;

    // Get the direction to the target
    sf::Vector2f direction = target.position - character.position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // If within target radius, stop completely
    if (distance < targetRadius) {
        result.linear = {0, 0};
        result.angular = 0;
        return result;
    }

    float targetSpeed;
    sf::Vector2f targetVelocity;

    // Determine Target Speed (Slow Down as it Approaches)
    if (distance > slowRadius) {
        targetSpeed = maxSpeed;  // Move at max speed
    } else {
        targetSpeed = maxSpeed * (distance / slowRadius);  // Slow down proportionally
    }

    // Normalize Direction & Apply Target Speed
    if (distance > 0) {
        targetVelocity = (direction / distance) * targetSpeed;
    } else {
        targetVelocity = {0, 0}; // No movement if already at target
    }

    // Compute the linear acceleration needed to reach the target velocity
    result.linear = (targetVelocity - character.velocity) / timeToTarget;

    // Ensure stopping completely when within threshold
    if (std::sqrt(result.linear.x * result.linear.x + result.linear.y * result.linear.y) < 0.01f &&
        std::sqrt(character.velocity.x * character.velocity.x + character.velocity.y * character.velocity.y) < 1.0f) {
        result.linear = {0, 0};  // Hard stop
    }

    // Clamp acceleration if needed
    float accelMagnitude = std::sqrt(result.linear.x * result.linear.x + result.linear.y * result.linear.y);
    if (accelMagnitude > maxAcceleration) {
        result.linear = (result.linear / accelMagnitude) * maxAcceleration;
    }

    // Angular acceleration is not needed for Arrive
    result.angular = 0;
    return result;
}
