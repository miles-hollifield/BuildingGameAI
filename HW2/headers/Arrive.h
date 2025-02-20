#ifndef ARRIVE_H
#define ARRIVE_H

#include "SteeringBehavior.h"

class Arrive : public SteeringBehavior {
public:
    float maxAcceleration;
    float maxSpeed;
    float targetRadius;
    float slowRadius;
    float timeToTarget;

    Arrive(float maxAcceleration = 100.0f, float maxSpeed = 200.0f, 
           float targetRadius = 5.0f, float slowRadius = 100.0f, 
           float timeToTarget = 0.1f);

    SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& target) override;
};

#endif // ARRIVE_H
