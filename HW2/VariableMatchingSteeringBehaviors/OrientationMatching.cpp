#include "OrientationMatching.h"

SteeringData OrientationMatching::calculateAcceleration(const Kinematic& character, const Kinematic& goal) {
  float angularDifference = goal.orientation - character.orientation;
  return SteeringData({0, 0}, angularDifference * maxAngularAcceleration);
}
