#ifndef ROTATIONMATCHING_H
#define ROTATIONMATCHING_H

#include "SteeringBehavior.h"

class RotationMatching : public SteeringBehavior {
public:
  float timeToTarget = 0.1f;

  SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& goal) override;
};

#endif // ROTATIONMATCHING_H
