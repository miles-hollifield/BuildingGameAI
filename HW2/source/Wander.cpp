#include "../headers/Wander.h"
#include "../headers/Constants.h"
#include <random>

// Constructor
Wander::Wander(float radius, float distance, float angleSmoothing)
  : wanderRadius(radius), wanderDistance(distance), wanderAngleSmoothing(angleSmoothing), wanderAngle(0) {}

// Compute Steering Behavior
SteeringData Wander::calculateAcceleration(Kinematic& character) {
  SteeringData result;

  // Step 1: Compute wander circle center
  sf::Vector2f circleCenter = character.position + character.velocity;
  float centerLength = std::sqrt(circleCenter.x * circleCenter.x + circleCenter.y * circleCenter.y);
  if (centerLength > 0) circleCenter = (circleCenter / centerLength) * wanderDistance;

  // Step 2: Adjust the wander angle gradually
  wanderAngle += randomBinomial() * wanderAngleSmoothing;

  // Step 3: Compute displacement using the wander angle
  float angleRad = wanderAngle * (3.14159265f / 180.0f);
  sf::Vector2f displacement = sf::Vector2f(std::cos(angleRad), std::sin(angleRad)) * wanderRadius;

  // Step 4: Compute wander target and steering force
  sf::Vector2f wanderTarget = circleCenter + displacement;
  result.linear = wanderTarget - character.position;

  // Normalize and scale acceleration
  float steerLength = std::sqrt(result.linear.x * result.linear.x + result.linear.y * result.linear.y);
  if (steerLength > 0) result.linear = (result.linear / steerLength) * MAX_ACCELERATION;

  // Adjust angular acceleration to rotate smoothly
  float targetOrientation = std::atan2(result.linear.y, result.linear.x) * (180.0f / 3.14159f);
  result.angular = (targetOrientation - character.orientation) * 0.05f; // Smaller value for smoother turns

  return result;
}

// Random value between -1 and 1 for jittering
float Wander::randomBinomial() {
  return (static_cast<float>(rand()) / RAND_MAX) - (static_cast<float>(rand()) / RAND_MAX);
}
