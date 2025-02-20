#ifndef VELOCITYMATCHING_H
#define VELOCITYMATCHING_H

#include "SteeringBehavior.h"

class VelocityMatching : public SteeringBehavior {
public:
    float maxAcceleration, timeToTarget;

    VelocityMatching(float maxAcceleration = 100.0f, float timeToTarget = 0.1f);

    SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& target) override;
};

#endif // VELOCITYMATCHING_H
