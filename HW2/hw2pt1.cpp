#include <iostream>
#include "PositionMatching.h"
#include "OrientationMatching.h"
#include "VelocityMatching.h"
#include "RotationMatching.h"

int main() {
  Kinematic character({0, 0}, {10, 5}, 0, 1);
  Kinematic goal({100, 100}, {0, 0}, 45, 2);

  PositionMatching positionBehavior;
  OrientationMatching orientationBehavior;
  VelocityMatching velocityBehavior;
  RotationMatching rotationBehavior;

  SteeringData posAccel = positionBehavior.calculateAcceleration(character, goal);
  SteeringData orientAccel = orientationBehavior.calculateAcceleration(character, goal);
  SteeringData velAccel = velocityBehavior.calculateAcceleration(character, goal);
  SteeringData rotAccel = rotationBehavior.calculateAcceleration(character, goal);

  std::cout << "Position Acceleration: (" << posAccel.linear.x << ", " << posAccel.linear.y << ")\n";
  std::cout << "Orientation Acceleration: " << orientAccel.angular << "\n";
  std::cout << "Velocity Acceleration: (" << velAccel.linear.x << ", " << velAccel.linear.y << ")\n";
  std::cout << "Rotation Acceleration: " << rotAccel.angular << "\n";

  return 0;
}
