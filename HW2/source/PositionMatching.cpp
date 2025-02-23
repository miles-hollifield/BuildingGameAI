#include "../headers/PositionMatching.h"

// Calculate the acceleration needed to match the character's position to the goal's position
SteeringData PositionMatching::calculateAcceleration(const Kinematic& character, const Kinematic& goal) {
  // Compute the direction vector toward the target's position
  sf::Vector2f direction = goal.position - character.position;
  
  // Calculate the magnitude of the direction vector
  float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);

  // If there's a distance to cover, normalize and apply max acceleration
  if (magnitude > 0) {
    direction /= magnitude; // Normalize direction
    direction *= maxAcceleration; // Scale by max acceleration
  }

  // Return the steering data with angular acceleration set to zero
  return {direction, 0.0f};
}