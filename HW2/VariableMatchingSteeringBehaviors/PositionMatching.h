#ifndef POSITIONMATCHING_H
#define POSITIONMATCHING_H

#include "SteeringBehavior.h"

class PositionMatching : public SteeringBehavior {
public:
  float maxAcceleration = 100.0f; // Maximum allowed acceleration

  SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& goal) override {
    sf::Vector2f direction = goal.position - character.position;
    return SteeringData(direction * maxAcceleration, 0);
  }
};

#endif // POSITIONMATCHING_H
