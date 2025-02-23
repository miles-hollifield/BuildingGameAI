/**
 * @file OrientationMatching.h
 * @brief Defines the OrientationMatching behavior for matching an entity's orientation to a target.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 *
 * Author: Miles Hollifield
 * Date: 2/23/2025
 */

#ifndef ORIENTATIONMATCHING_H
#define ORIENTATIONMATCHING_H

#include "SteeringBehavior.h"

/**
 * @class OrientationMatching
 * @brief Matches an entity's orientation to align with a target's orientation.
 */
class OrientationMatching : public SteeringBehavior {
public:
  float maxAngularAcceleration = 5.0f; // Maximum angular acceleration

  /**
   * @brief Calculates the angular acceleration needed to match the target's orientation.
   * @param character The entity performing the behavior.
   * @param goal The target entity with the desired orientation.
   * @return A SteeringData object containing angular acceleration to match the orientation.
   */
  SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& goal) override;
};

#endif // ORIENTATIONMATCHING_H