#ifndef ORIENTATIONMATCHING_H
#define ORIENTATIONMATCHING_H

#include "SteeringBehavior.h"

class OrientationMatching : public SteeringBehavior {
public:
  float maxAngularAcceleration = 5.0f; // Maximum allowed angular acceleration

  SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& goal) override {
    float angularDifference = goal.orientation - character.orientation;
    return SteeringData({0, 0}, angularDifference * maxAngularAcceleration);
  }
};

#endif // ORIENTATIONMATCHING_H
