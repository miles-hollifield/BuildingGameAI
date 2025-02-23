/**
 * @file Align.h
 * @brief Defines the Align behavior for smoothly matching an entity's orientation to a target's orientation.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 *
 * Author: Miles Hollifield
 * Date: 2/23/2025
 */

#ifndef ALIGN_H
#define ALIGN_H

#include "SteeringBehavior.h"

/**
 * @class Align
 * @brief Matches an entity's orientation smoothly to align with a target's orientation.
 */
class Align : public SteeringBehavior {
public:
    float maxAngularAcceleration; // Maximum angular acceleration
    float maxRotation; // Maximum rotation speed
    float targetRadius; // Radius for target acquisition
    float slowRadius;  // Radius for slowing down
    float timeToTarget;  // Time to reach the target orientation

    /**
     * @brief Constructor to initialize alignment behavior parameters.
     * @param maxAngularAcceleration Maximum angular acceleration.
     * @param maxRotation Maximum rotation speed.
     * @param targetRadius Radius within which no rotation is needed.
     * @param slowRadius Radius within which rotation slows down.
     * @param timeToTarget Time to achieve the target rotation.
     */
    Align(float maxAngularAcceleration = 5.0f, float maxRotation = 180.0f, 
          float targetRadius = 1.0f, float slowRadius = 30.0f, float timeToTarget = 0.1f);

    /**
     * @brief Calculates angular acceleration to smoothly align with the target's orientation.
     * @param character The entity performing the behavior.
     * @param target The target entity with the desired orientation.
     * @return A SteeringData object containing angular acceleration to align orientation.
     */      
    SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& target) override;
};

#endif // ALIGN_H
