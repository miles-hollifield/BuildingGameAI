#ifndef POSITIONMATCHING_H
#define POSITIONMATCHING_H

#include "SteeringBehavior.h"

class PositionMatching : public SteeringBehavior {
public:
  float maxAcceleration = 100.0f;

  SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& goal) override;
};

#endif // POSITIONMATCHING_H