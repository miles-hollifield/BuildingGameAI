#ifndef ROTATIONMATCHING_H
#define ROTATIONMATCHING_H

#include "SteeringBehavior.h"

class RotationMatching : public SteeringBehavior {
public:
    float maxAngularAcceleration, timeToTarget;

    RotationMatching(float maxAngularAcceleration = 5.0f, float timeToTarget = 0.1f);

    SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& target) override;
};

#endif // ROTATIONMATCHING_H
