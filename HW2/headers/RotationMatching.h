/**
 * @file RotationMatching.h
 * @brief Defines the RotationMatching behavior for matching an entity's rotational speed to a target.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 *
 * Author: Miles Hollifield
 * Date: 2/23/2025
 */

#ifndef ROTATIONMATCHING_H
#define ROTATIONMATCHING_H

#include "SteeringBehavior.h"

/**
 * @class RotationMatching
 * @brief Matches an entity's rotation speed to align with a target's rotational speed.
 */
class RotationMatching : public SteeringBehavior {
public:
  float timeToTarget = 0.1f; // Time to reach the target rotation speed

  /**
   * @brief Calculates the angular acceleration needed to match the target's rotational speed.
   * @param character The entity performing the behavior.
   * @param goal The target entity with the desired rotational speed.
   * @return A SteeringData object containing angular acceleration to match the rotation.
   */
  SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& goal) override;
};

#endif // ROTATIONMATCHING_H