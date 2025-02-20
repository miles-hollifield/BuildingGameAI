#include "../headers/OrientationMatching.h"
#include <cmath>

SteeringData OrientationMatching::calculateAcceleration(const Kinematic& character, const Kinematic& target) {
    SteeringData result;

    // Compute the rotation difference
    float rotation = target.orientation - character.orientation;

    // Map rotation to (-180, 180) degrees
    while (rotation > 180) rotation -= 360;
    while (rotation < -180) rotation += 360;

    // Apply max angular acceleration in the direction of needed rotation
    result.angular = (rotation > 0 ? 1 : -1) * maxAngularAcceleration;

    result.linear = {0, 0};  // No linear acceleration in orientation matching
    return result;
}
