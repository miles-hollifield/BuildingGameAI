/**
 * @file PositionMatching.h
 * @brief Defines the PositionMatching behavior for moving an entity to match a target's position.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 *
 * Author: Miles Hollifield
 * Date: 2/23/2025
 */

#ifndef POSITIONMATCHING_H
#define POSITIONMATCHING_H

#include "SteeringBehavior.h"

/**
 * @class PositionMatching
 * @brief Matches an entity's position to align with a target's position.
 */
class PositionMatching : public SteeringBehavior {
public:
  float maxAcceleration = 100.0f; // Maximum linear acceleration

  /**
   * @brief Calculates the linear acceleration needed to match the target's position.
   * @param character The entity performing the behavior.
   * @param goal The target entity with the desired position.
   * @return A SteeringData object containing linear acceleration to match the position.
   */
  SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& goal) override;
};

#endif // POSITIONMATCHING_H