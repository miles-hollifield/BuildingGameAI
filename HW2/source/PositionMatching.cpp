#include "../headers/PositionMatching.h"

SteeringData PositionMatching::calculateAcceleration(const Kinematic& character, const Kinematic& goal) {
  // Compute direction toward the target
  sf::Vector2f direction = goal.position - character.position;
  
  // Normalize direction and apply max acceleration
  float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
  if (magnitude > 0) {
    direction /= magnitude; // Normalize
    direction *= maxAcceleration;
  }

  return {direction, 0.0f}; // Only linear acceleration is affected
}