/**
 * @file VelocityMatching.h
 * @brief Defines the VelocityMatching behavior for matching an entity's velocity to a target.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 *
 * Author: Miles Hollifield
 * Date: 2/23/2025
 */

#ifndef VELOCITYMATCHING_H
#define VELOCITYMATCHING_H

#include "SteeringBehavior.h"

/**
 * @class VelocityMatching
 * @brief Matches an entity's velocity to align with a target's velocity.
 */
class VelocityMatching : public SteeringBehavior {
public:
  float timeToTarget = 0.1f; // Time to reach the target velocity

  /**
   * @brief Calculates the linear acceleration needed to match the target's velocity.
   * @param character The entity performing the behavior.
   * @param goal The target entity with the desired velocity.
   * @return A SteeringData object containing linear acceleration to match the velocity.
   */
  SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& goal) override;
};

#endif // VELOCITYMATCHING_H