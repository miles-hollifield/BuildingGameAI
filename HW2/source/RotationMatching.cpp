#include "../headers/RotationMatching.h"

// Calculate the acceleration needed to match the character's rotation to the goal's rotation
SteeringData RotationMatching::calculateAcceleration(const Kinematic& character, const Kinematic& goal) {
  // Compute the desired angular acceleration to match the target's rotation
  float angularAcceleration = (goal.rotation - character.rotation) / timeToTarget;

  // Return the steering data with linear acceleration set to zero
  return {{0, 0}, angularAcceleration};
}