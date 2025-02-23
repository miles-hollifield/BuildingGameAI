#include "../headers/Align.h"
#include <cmath>

// Constructor
Align::Align(float maxAngularAcceleration, float maxRotation, float targetRadius, float slowRadius, float timeToTarget)
    : maxAngularAcceleration(maxAngularAcceleration), maxRotation(maxRotation),
      targetRadius(targetRadius), slowRadius(slowRadius), timeToTarget(timeToTarget) {}

SteeringData Align::calculateAcceleration(const Kinematic& character, const Kinematic& target) {
    SteeringData result;

    // Calculate the difference in orientation
    float rotation = target.orientation - character.orientation;

    // Normalize the rotation to the range (-180, 180) degrees to avoid unnecessary large rotations
    while (rotation > 180) rotation -= 360;
    while (rotation < -180) rotation += 360;
    float rotationSize = std::abs(rotation);

    // If we are within the target radius, return no steering (stop rotating)
    if (rotationSize < targetRadius) {
        result.angular = 0;
        return result;
    }

    float targetRotation;

    // If outside the slowRadius, rotate at max speed
    if (rotationSize > slowRadius) {
        targetRotation = maxRotation;
    } 
    // Otherwise, scale rotation speed
    else {
        targetRotation = maxRotation * (rotationSize / slowRadius);
    }

    // Apply rotation direction
    targetRotation *= (rotation / rotationSize);

    // Compute required angular acceleration
    result.angular = (targetRotation - character.rotation) / timeToTarget;

    // Clamp angular acceleration if needed
    float angularAcceleration = std::abs(result.angular);
    if (angularAcceleration > maxAngularAcceleration) {
        result.angular /= angularAcceleration; // Normalize to direction
        result.angular *= maxAngularAcceleration;
    }

    // Linear acceleration is not needed for Align
    result.linear = {0, 0};
    return result;
}
