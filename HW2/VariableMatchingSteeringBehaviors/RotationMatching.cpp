#include "RotationMatching.h"

SteeringData RotationMatching::calculateAcceleration(const Kinematic& character, const Kinematic& goal) {
  float angularAcceleration = (goal.rotation - character.rotation) / timeToTarget;
  return SteeringData({0, 0}, angularAcceleration);
}
