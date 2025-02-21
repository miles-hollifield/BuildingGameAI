#include "../headers/OrientationMatching.h"

SteeringData OrientationMatching::calculateAcceleration(const Kinematic& character, const Kinematic& goal) {
  // Calculate angular difference
  float rotationDiff = goal.orientation - character.orientation;

  // Normalize to range -180 to 180 degrees
  while (rotationDiff > 180) rotationDiff -= 360;
  while (rotationDiff < -180) rotationDiff += 360;

  float angularAcceleration = maxAngularAcceleration * (rotationDiff / 180.0f);
  return {{0, 0}, angularAcceleration};
}