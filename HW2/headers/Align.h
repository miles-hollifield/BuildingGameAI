#ifndef ALIGN_H
#define ALIGN_H

#include "SteeringBehavior.h"

class Align : public SteeringBehavior {
public:
    float maxAngularAcceleration;
    float maxRotation;
    float targetRadius;
    float slowRadius;
    float timeToTarget;

    Align(float maxAngularAcceleration = 5.0f, float maxRotation = 180.0f, 
          float targetRadius = 1.0f, float slowRadius = 30.0f, float timeToTarget = 0.1f);

    SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& target) override;
};

#endif // ALIGN_H
