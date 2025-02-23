#include "../headers/OrientationMatching.h"

// Calculate the acceleration needed to match the character's orientation to the goal's orientation
SteeringData OrientationMatching::calculateAcceleration(const Kinematic& character, const Kinematic& goal) {
  // Calculate the difference between the target and character orientation
  float rotationDiff = goal.orientation - character.orientation;

  // Normalize to range -180 to 180 degrees
  while (rotationDiff > 180) rotationDiff -= 360;
  while (rotationDiff < -180) rotationDiff += 360;

  // Scale the angular difference proportionally to max angular acceleration
  float angularAcceleration = maxAngularAcceleration * (rotationDiff / 180.0f);

  // Return the steering data with linear acceleration set to zero
  return {{0, 0}, angularAcceleration};
}