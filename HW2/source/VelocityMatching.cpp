#include "../headers/VelocityMatching.h"

SteeringData VelocityMatching::calculateAcceleration(const Kinematic& character, const Kinematic& goal) {
  // Compute desired acceleration to match velocity
  SteeringData steering;
  steering.linear = (goal.velocity - character.velocity) / timeToTarget;
  steering.angular = 0; // No angular acceleration for velocity matching
  return steering;
}
