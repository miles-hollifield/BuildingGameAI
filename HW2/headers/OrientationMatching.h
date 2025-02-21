#ifndef ORIENTATIONMATCHING_H
#define ORIENTATIONMATCHING_H

#include "SteeringBehavior.h"

class OrientationMatching : public SteeringBehavior {
public:
  float maxAngularAcceleration = 5.0f;

  SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& goal) override;
};

#endif // ORIENTATIONMATCHING_H