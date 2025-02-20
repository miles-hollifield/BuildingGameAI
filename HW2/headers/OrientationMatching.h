#ifndef ORIENTATIONMATCHING_H
#define ORIENTATIONMATCHING_H

#include "SteeringBehavior.h"

class OrientationMatching : public SteeringBehavior {
public:
    float maxAngularAcceleration, maxRotation, targetRadius, slowRadius, timeToTarget;

    OrientationMatching(float maxAngularAcceleration = 5.0f, float maxRotation = 180.0f,
                        float targetRadius = 1.0f, float slowRadius = 10.0f,
                        float timeToTarget = 0.1f);

    SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& target) override;
};

#endif // ORIENTATIONMATCHING_H
