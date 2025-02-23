/**
 * @file SteeringData.h
 * @brief Defines the SteeringData structure for linear and angular accelerations.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 *
 * Author: Miles Hollifield
 * Date: 2/23/2025
 */

#ifndef STEERINGDATA_H
#define STEERINGDATA_H

#include <SFML/System.hpp>

/**
 * @struct SteeringData
 * @brief Represents the output of a steering behavior in terms of linear and angular acceleration.
 */
struct SteeringData {
    sf::Vector2f linear; // Linear acceleration
    float angular; // Angular acceleration

    /**
     * @brief Default constructor to initialize steering data.
     * @param lin Initial linear acceleration.
     * @param ang Initial angular acceleration.
     */
    SteeringData(sf::Vector2f lin = {0, 0}, float ang = 0) : linear(lin), angular(ang) {}
};

#endif // STEERINGDATA_H
