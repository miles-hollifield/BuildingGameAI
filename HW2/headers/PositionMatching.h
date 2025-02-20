#ifndef POSITIONMATCHING_H
#define POSITIONMATCHING_H

#include "SteeringBehavior.h"

class PositionMatching : public SteeringBehavior {
public:
    float maxAcceleration, maxSpeed, targetRadius, slowRadius, timeToTarget;

    PositionMatching(float maxAcceleration = 100.0f, float maxSpeed = 200.0f,
                     float targetRadius = 5.0f, float slowRadius = 50.0f,
                     float timeToTarget = 0.1f);

    SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& target) override;
};

#endif // POSITIONMATCHING_H
