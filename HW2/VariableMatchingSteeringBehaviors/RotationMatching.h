#ifndef ROTATIONMATCHING_H
#define ROTATIONMATCHING_H

#include "SteeringBehavior.h"

class RotationMatching : public SteeringBehavior {
public:
  float timeToTarget = 0.1f; // Smooth transition factor

  SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& goal) override {
    float angularAcceleration = (goal.rotation - character.rotation) / timeToTarget;
    return SteeringData({0, 0}, angularAcceleration);
  }
};

#endif // ROTATIONMATCHING_H
