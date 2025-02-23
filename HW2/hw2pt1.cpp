/**
 * @file hw2pt1.cpp
 * @brief Implements a velocity matching behavior, where a character sprite matches the velocity of the mouse cursor.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 *
 * Author: Miles Hollifield
 * Date: 2/23/2025
 */

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cmath>

// Include necessary headers
#include "headers/Kinematic.h"
#include "headers/VelocityMatching.h"

// Constants
constexpr float WINDOW_WIDTH = 640; // Window width
constexpr float WINDOW_HEIGHT = 480; // Window height
constexpr float TIME_STEP = 0.1f;  // Time step for velocity calculation
constexpr float SPRITE_SCALE = 0.1f; // Adjust the scale for `boid.png`

// Helper function to compute velocity from mouse movement
sf::Vector2f getMouseVelocity(sf::Vector2f lastMousePos, sf::Vector2f currentMousePos, float deltaTime) {
  if (deltaTime > 0)
    return (currentMousePos - lastMousePos) / deltaTime; // Calculate the change in position over time
  return {0, 0}; // Return zero velocity if time step is invalid
}

int main() {
  // Create an SFML window
  sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Part 1: Variable Matching Steering Behaviors");

  // Load boid texture
  sf::Texture boidTexture;
  if (!boidTexture.loadFromFile("boid.png")) {
    std::cerr << "Failed to load boid.png!" << std::endl;
    return -1;
  }

  // Character setup
  sf::Sprite character;
  character.setTexture(boidTexture);
  character.setScale(SPRITE_SCALE, SPRITE_SCALE);
  character.setOrigin(boidTexture.getSize().x / 2, boidTexture.getSize().y / 2);
  sf::Vector2f characterPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

  // Mouse tracking
  sf::Vector2f lastMousePos = sf::Vector2f(sf::Mouse::getPosition(window));

  // Create Kinematic objects for the character and mouse
  Kinematic characterKinematic(characterPosition, {0, 0}, 0, 0);
  Kinematic mouseKinematic(lastMousePos, {0, 0}, 0, 0);

  // Velocity Matching Behavior
  VelocityMatching velocityMatching;

  // Start the game clock
  sf::Clock clock;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close(); // Close window
    }

    // Get delta time
    float deltaTime = clock.restart().asSeconds();

    // Get Mouse Position & Compute Velocity
    sf::Vector2f currentMousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
    mouseKinematic.velocity = getMouseVelocity(lastMousePos, currentMousePos, deltaTime);
    lastMousePos = currentMousePos; // Update last mouse position

    // Apply velocity matching to calculate acceleration
    SteeringData velocityAccel = velocityMatching.calculateAcceleration(characterKinematic, mouseKinematic);

    // Update Character Kinematics
    characterKinematic.velocity += velocityAccel.linear * deltaTime;
    characterKinematic.update(deltaTime); // Update position and orientation based on velocity

    // Update SFML Sprite
    character.setPosition(characterKinematic.position);

    // Set sprite rotation based on movement direction
    if (characterKinematic.velocity.x != 0 || characterKinematic.velocity.y != 0) {
      float angle = std::atan2(characterKinematic.velocity.y, characterKinematic.velocity.x) * (180.0f / 3.14159265f);
      character.setRotation(angle);
    }

    // Render
    window.clear(sf::Color::White);
    window.draw(character);
    window.display();
  }

  return 0;
}