/**
 * @file SteeringBehavior.h
 * @brief Abstract base class for all steering behaviors.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 *
 * Author: Miles Hollifield
 * Date: 2/23/2025
 */

#ifndef STEERINGBEHAVIOR_H
#define STEERINGBEHAVIOR_H

#include "Kinematic.h"
#include "SteeringData.h"

/**
 * @class SteeringBehavior
 * @brief Abstract base class that defines a generic interface for steering behaviors.
 */
class SteeringBehavior {
public:
    virtual ~SteeringBehavior() = default;

    /**
     * @brief Pure virtual function that calculates the acceleration needed for a specific behavior.
     * @param character The entity applying the steering behavior.
     * @param target The target entity or position for the behavior.
     * @return A SteeringData object containing linear and angular acceleration.
     */
    virtual SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& target) = 0;
};

#endif // STEERINGBEHAVIOR_H
