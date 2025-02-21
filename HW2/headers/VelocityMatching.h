#ifndef VELOCITYMATCHING_H
#define VELOCITYMATCHING_H

#include "SteeringBehavior.h"

class VelocityMatching : public SteeringBehavior {
public:
  float timeToTarget = 0.1f;

  SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& goal) override;
};

#endif // VELOCITYMATCHING_H