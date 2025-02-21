#ifndef WANDER_H
#define WANDER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include "../headers/Kinematic.h"
#include "../headers/SteeringData.h"
#include "../headers/Constants.h"

class Wander {
public:
  Wander(float radius, float distance, float angleSmoothing);
  SteeringData calculateAcceleration(Kinematic& character);

private:
  float wanderRadius;
  float wanderDistance;
  float wanderAngleSmoothing;
  float wanderAngle; // Stores the current angle offset

  float randomBinomial();
};

#endif // WANDER_H
