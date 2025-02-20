#include "../headers/OrientationMatching.h"
#include <cmath>

// Constructor
OrientationMatching::OrientationMatching(float maxAngularAcceleration, float maxRotation, float targetRadius, float slowRadius, float timeToTarget)
    : maxAngularAcceleration(maxAngularAcceleration), maxRotation(maxRotation), targetRadius(targetRadius), slowRadius(slowRadius), timeToTarget(timeToTarget) {}

SteeringData OrientationMatching::calculateAcceleration(const Kinematic& character, const Kinematic& target) {
    SteeringData result;

    // Compute rotation difference
    float rotation = target.orientation - character.orientation;

    // Map rotation to (-180, 180) degrees
    while (rotation > 180) rotation -= 360;
    while (rotation < -180) rotation += 360;

    float rotationSize = std::abs(rotation);

    // If within target radius, stop rotating
    if (rotationSize < targetRadius) {
        return SteeringData({0, 0}, 0);
    }

    // Determine target rotation speed
    float targetRotation = (rotationSize > slowRadius) ? maxRotation : maxRotation * (rotationSize / slowRadius);

    // Apply direction
    targetRotation *= (rotation / rotationSize);

    // Compute acceleration needed
    result.angular = (targetRotation - character.rotation) / timeToTarget;

    // Clamp angular acceleration
    if (std::abs(result.angular) > maxAngularAcceleration) {
        result.angular = (result.angular / std::abs(result.angular)) * maxAngularAcceleration;
    }

    result.linear = {0, 0};
    return result;
}
