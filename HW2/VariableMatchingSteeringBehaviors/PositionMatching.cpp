#include "PositionMatching.h"

SteeringData PositionMatching::calculateAcceleration(const Kinematic& character, const Kinematic& goal) {
  sf::Vector2f direction = goal.position - character.position;
  return SteeringData(direction * maxAcceleration, 0);
}
