#ifndef POSITIONMATCHING_H
#define POSITIONMATCHING_H

#include "SteeringBehavior.h"

class PositionMatching : public SteeringBehavior {
public:
    float maxAcceleration;  // Maximum acceleration allowed

    PositionMatching(float maxAcceleration = 100.0f)
        : maxAcceleration(maxAcceleration) {}

    SteeringData calculateAcceleration(const Kinematic& character, const Kinematic& target) override;
};

#endif // POSITIONMATCHING_H
