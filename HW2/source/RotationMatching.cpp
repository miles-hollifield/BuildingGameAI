#include "../headers/RotationMatching.h"
#include <cmath>

// Constructor
RotationMatching::RotationMatching(float maxAngularAcceleration, float timeToTarget)
    : maxAngularAcceleration(maxAngularAcceleration), timeToTarget(timeToTarget) {}

SteeringData RotationMatching::calculateAcceleration(const Kinematic& character, const Kinematic& target) {
    SteeringData result;

    // Compute rotation difference
    float rotation = target.rotation - character.rotation;

    // Compute acceleration needed to match rotation
    result.angular = rotation / timeToTarget;

    // Clamp angular acceleration
    if (std::abs(result.angular) > maxAngularAcceleration) {
        result.angular = (result.angular / std::abs(result.angular)) * maxAngularAcceleration;
    }

    result.linear = {0, 0};
    return result;
}
