/**
 * @file Arrive.h
 * @brief Defines the Arrive behavior for smoothly moving an entity towards a target position.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - AI Tools: OpenAI's ChatGPT
 *
 * Author: Miles Hollifield
 * Date: 2/23/2025
 */

/** OpenAI's ChatGPT was used to suggest a template header file for Arrive's
 * implementation. The following prompt was used: "Create a template header file 
 * for a class that implements Arrive behavior in C++."
 */

 #ifndef ARRIVE_H
 #define ARRIVE_H
 
 #include "SteeringBehavior.h"
 
 /**
  * @class Arrive
  * @brief Moves an entity smoothly towards a target position with gradual deceleration.
  */
 class Arrive : public SteeringBehavior {
 public:
     float maxAcceleration; // Maximum linear acceleration
     float maxSpeed; // Maximum speed
     float targetRadius; // Radius for target acquisition
     float slowRadius; // Radius for slowing down
     float timeToTarget; // Time to reach the target velocity
 
     /**
      * @brief Constructor to initialize arrival behavior parameters.
      * @param maxAcceleration Maximum linear acceleration.
      * @param maxSpeed Maximum speed.
      * @param targetRadius Radius within which no further movement is needed.
      * @param slowRadius Radius within which slowing down starts.
      * @param timeToTarget Time to achieve the target velocity.
      */
     Arrive(float maxAcceleration = 100.0f, float maxSpeed = 200.0f, 
            float targetRadius = 5.0f, float slowRadius = 100.0f, 
            float timeToTarget = 0.1f);
 
     /**
      * @brief Calculates linear acceleration to smoothly approach the target's position.
      * @param character The entity performing the behavior.
      * @param target The target entity with the desired position.
      * @return A SteeringData object containing linear acceleration to arrive at the target.
      */       
     SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& target) override;
 };
 
 #endif // ARRIVE_H
 
 /** End ChatGPT citation */
 