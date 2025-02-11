#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include "VariableMatchingSteeringBehaviors/Kinematic.h"
#include "VariableMatchingSteeringBehaviors/PositionMatching.h"
#include "VariableMatchingSteeringBehaviors/OrientationMatching.h"
#include "VariableMatchingSteeringBehaviors/VelocityMatching.h"
#include "VariableMatchingSteeringBehaviors/RotationMatching.h"
#include "VariableMatchingSteeringBehaviors/SteeringBehavior.h"

const float WINDOW_WIDTH = 800;
const float WINDOW_HEIGHT = 600;

// Converts mouse position to velocity (helper function)
sf::Vector2f getMouseVelocity(sf::Vector2f lastMousePos, sf::Vector2f currentMousePos, float deltaTime) {
  if (deltaTime > 0)
    return (currentMousePos - lastMousePos) / deltaTime;
  return {0, 0};
}

int main() {
  sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Variable Matching Steering Behaviors");

  // Character Setup
  sf::CircleShape character(20);
  character.setFillColor(sf::Color::Blue);
  sf::Vector2f characterPosition(100, 100);

  // Goal (Target) Setup
  sf::CircleShape goal(10);
  goal.setFillColor(sf::Color::Red);
  sf::Vector2f goalPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

  // Mouse Position Tracking
  sf::Vector2f lastMousePos = sf::Vector2f(sf::Mouse::getPosition(window));

  // Kinematic Objects
  Kinematic characterKinematic(characterPosition, {0, 0}, 0, 0);
  Kinematic goalKinematic(goalPosition, {0, 0}, 0, 0);

  // Steering Behaviors
  PositionMatching positionBehavior;
  OrientationMatching orientationBehavior;
  VelocityMatching velocityBehavior;

  sf::Clock clock;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    // Get delta time
    float deltaTime = clock.restart().asSeconds();

    // Get Mouse Position and Velocity Matching
    sf::Vector2f currentMousePos = sf::Vector2f(sf::Mouse::getPosition(window));
    Kinematic mouseKinematic(currentMousePos, getMouseVelocity(lastMousePos, currentMousePos, deltaTime), 0, 0);
    lastMousePos = currentMousePos;

    // Apply Steering Behaviors
    SteeringData positionAccel = positionBehavior.calculateAcceleration(characterKinematic, goalKinematic);
    SteeringData orientationAccel = orientationBehavior.calculateAcceleration(characterKinematic, goalKinematic);
    SteeringData velocityAccel = velocityBehavior.calculateAcceleration(characterKinematic, mouseKinematic);

    // Update Character Kinematic
    characterKinematic.velocity += velocityAccel.linear * deltaTime;
    characterKinematic.rotation += orientationAccel.angular * deltaTime;
    characterKinematic.update(deltaTime);

    // Update SFML Shapes
    character.setPosition(characterKinematic.position);
    goal.setPosition(goalKinematic.position);

    window.clear(sf::Color::White);
    window.draw(goal);
    window.draw(character);
    window.display();
  }

  return 0;
}
