#ifndef KINEMATIC_H
#define KINEMATIC_H

#include <SFML/System.hpp>

struct Kinematic {
  sf::Vector2f position;
  sf::Vector2f velocity;
  float orientation;
  float rotation;

  Kinematic(sf::Vector2f pos = {0, 0}, sf::Vector2f vel = {0, 0}, float orient = 0, float rot = 0)
      : position(pos), velocity(vel), orientation(orient), rotation(rot) {}
};

#endif // KINEMATIC_H
