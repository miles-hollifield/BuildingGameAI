#include "../headers/Align.h"
#include <cmath>

// Constructor
Align::Align(float maxAngularAcceleration, float maxRotation, float targetRadius, float slowRadius, float timeToTarget)
    : maxAngularAcceleration(maxAngularAcceleration), maxRotation(maxRotation),
      targetRadius(targetRadius), slowRadius(slowRadius), timeToTarget(timeToTarget) {}

SteeringData Align::calculateAcceleration(const Kinematic& character, const Kinematic& target) {
    SteeringData result;

    // Step 1: Compute the naive direction to the target orientation
    float rotation = target.orientation - character.orientation;

    // Step 2: Map the result to the (-180, 180) interval
    while (rotation > 180) rotation -= 360;
    while (rotation < -180) rotation += 360;

    float rotationSize = std::abs(rotation);

    // Step 3: If we are within the target radius, return no steering (stop rotating)
    if (rotationSize < targetRadius) {
        result.angular = 0;
        return result;
    }

    float targetRotation;

    // Step 4: If outside the slowRadius, rotate at max speed
    if (rotationSize > slowRadius) {
        targetRotation = maxRotation;
    } 
    // Step 5: Otherwise, scale rotation speed
    else {
        targetRotation = maxRotation * (rotationSize / slowRadius);
    }

    // Step 6: Apply rotation direction
    targetRotation *= (rotation / rotationSize);

    // Step 7: Compute required angular acceleration
    result.angular = (targetRotation - character.rotation) / timeToTarget;

    // Step 8: Clamp angular acceleration if needed
    float angularAcceleration = std::abs(result.angular);
    if (angularAcceleration > maxAngularAcceleration) {
        result.angular /= angularAcceleration; // Normalize
        result.angular *= maxAngularAcceleration;
    }

    // Step 9: Linear acceleration is not needed for Align
    result.linear = {0, 0};
    return result;
}
