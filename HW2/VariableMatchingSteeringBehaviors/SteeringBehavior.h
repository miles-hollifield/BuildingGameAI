#ifndef STEERINGBEHAVIOR_H
#define STEERINGBEHAVIOR_H

#include "Kinematic.h"
#include "SteeringData.h"

class SteeringBehavior {
public:
  virtual ~SteeringBehavior() = default;

  virtual SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& goal) = 0;
};

#endif // STEERINGBEHAVIOR_H
