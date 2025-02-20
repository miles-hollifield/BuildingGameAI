#ifndef ORIENTATIONMATCHING_H
#define ORIENTATIONMATCHING_H

#include "SteeringBehavior.h"

class OrientationMatching : public SteeringBehavior {
public:
    float maxAngularAcceleration;  // Maximum allowed angular acceleration

    OrientationMatching(float maxAngularAcceleration = 5.0f)
        : maxAngularAcceleration(maxAngularAcceleration) {}

    SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& target) override;
};

#endif // ORIENTATIONMATCHING_H
