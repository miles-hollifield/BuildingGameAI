#ifndef STEERINGDATA_H
#define STEERINGDATA_H

#include <SFML/System.hpp>

// Holds Steering Output (Linear & Angular Acceleration)
struct SteeringData {
  sf::Vector2f linear; // Linear acceleration
  float angular;       // Angular acceleration

  // Default Constructor
  SteeringData(sf::Vector2f lin = {0, 0}, float ang = 0) : linear(lin), angular(ang) {}
};

#endif // STEERINGDATA_H
