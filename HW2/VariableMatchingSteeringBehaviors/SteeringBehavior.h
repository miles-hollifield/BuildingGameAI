#ifndef STEERINGBEHAVIOR_H
#define STEERINGBEHAVIOR_H

#include "Kinematic.h"
#include "SteeringData.h"

// Abstract Base Class for Steering Behaviors
class SteeringBehavior {
public:
  virtual ~SteeringBehavior() = default;

  // Pure Virtual Function for Acceleration Calculation
  virtual SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& goal) = 0;
};

#endif // STEERINGBEHAVIOR_H
