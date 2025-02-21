#include "../headers/RotationMatching.h"

SteeringData RotationMatching::calculateAcceleration(const Kinematic& character, const Kinematic& goal) {
  // Compute desired angular acceleration
  float angularAcceleration = (goal.rotation - character.rotation) / timeToTarget;
  return {{0, 0}, angularAcceleration};
}