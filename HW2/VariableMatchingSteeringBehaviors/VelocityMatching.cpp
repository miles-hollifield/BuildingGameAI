#include "VelocityMatching.h"

SteeringData VelocityMatching::calculateAcceleration(const Kinematic& character, const Kinematic& goal) {
  sf::Vector2f acceleration = (goal.velocity - character.velocity) / timeToTarget;
  return SteeringData(acceleration, 0);
}
