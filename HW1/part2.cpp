/**
 * Part 2: Moving a Sprite
 * 
 * This program demonstrates how to move a sprite across the screen using SFML.
 * The sprite starts at the top-left corner and moves left to right across the window,
 * resetting its position when it reaches the right edge of the screen. Movement
 * is smooth and frame-rate independent, achieved using deltaTime.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * 
 * Author: Miles Hollifield
 * Date: 1/26/2025
 */

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

int main() {
  // Create the main window in VGA resolution (640x480)
  sf::RenderWindow window(sf::VideoMode(640, 480), "Moving a Sprite");

  // Load the sprite texture
  sf::Texture texture;
  if (!texture.loadFromFile("boid-sm.png")) {
    std::cerr << "Failed to load sprite texture!" << std::endl;
    return -1;
  }

  // Create the sprite
  sf::Sprite sprite(texture);

  // Set initial position of the sprite
  sprite.setPosition(0.0f, 25.0f); // Start near the top left corner

  // Movement speed (pixels per second)
  const float speed = 200.0f;

  // Clock to track deltaTime
  sf::Clock clock;

  while (window.isOpen()) {
    // Handle events
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    // Calculate deltaTime (time elapsed since the last frame)
    float deltaTime = clock.restart().asSeconds();

    // Update the sprite position
    sprite.move(speed * deltaTime, 0.0f); // Speed is scaled by deltaTime

    // Reset position if sprite moves out of bounds
    if (sprite.getPosition().x > 640) {
      sprite.setPosition(0.f, sprite.getPosition().y);
    }

    // Clear the screen
    window.clear(sf::Color::White);

    // Draw the sprite
    window.draw(sprite);

    // Update the window
    window.display();
  }

  return 0;
}