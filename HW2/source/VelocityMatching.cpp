#include "../headers/VelocityMatching.h"
#include <cmath>

// Constructor
VelocityMatching::VelocityMatching(float maxAcceleration, float timeToTarget)
    : maxAcceleration(maxAcceleration), timeToTarget(timeToTarget) {}

SteeringData VelocityMatching::calculateAcceleration(const Kinematic& character, const Kinematic& target) {
    SteeringData result;

    // Compute acceleration needed to match velocity
    result.linear = (target.velocity - character.velocity) / timeToTarget;

    // Clamp acceleration if needed
    float accelMagnitude = std::sqrt(result.linear.x * result.linear.x + result.linear.y * result.linear.y);
    if (accelMagnitude > maxAcceleration) {
        result.linear = (result.linear / accelMagnitude) * maxAcceleration;
    }

    result.angular = 0;
    return result;
}
