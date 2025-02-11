#ifndef VELOCITYMATCHING_H
#define VELOCITYMATCHING_H

#include "SteeringBehavior.h"

class VelocityMatching : public SteeringBehavior {
public:
  float timeToTarget = 0.1f; // Smooth transition factor

  SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& goal) override {
    sf::Vector2f acceleration = (goal.velocity - character.velocity) / timeToTarget;
    return SteeringData(acceleration, 0);
  }
};

#endif // VELOCITYMATCHING_H
