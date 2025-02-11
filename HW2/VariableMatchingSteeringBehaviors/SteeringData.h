#ifndef STEERINGDATA_H
#define STEERINGDATA_H

#include <SFML/System.hpp>

struct SteeringData {
  sf::Vector2f linear; // Linear acceleration
  float angular;       // Angular acceleration

  SteeringData(sf::Vector2f lin = {0, 0}, float ang = 0) : linear(lin), angular(ang) {}
};

#endif // STEERINGDATA_H
