#include "../headers/VelocityMatching.h"

// Calculate the acceleration needed to match the character's velocity to the goal's velocity
SteeringData VelocityMatching::calculateAcceleration(const Kinematic& character, const Kinematic& goal) {
  // Compute the linear acceleration needed to match the target's velocity
  SteeringData steering;

  // Calculate the linear acceleration
  steering.linear = (goal.velocity - character.velocity) / timeToTarget;
  // Set angular acceleration to zero
  steering.angular = 0;

  // Return the steering data
  return steering;
}