/**
 * Part 3: Multiple Sprites and More Complex Movement
 *
 * This program demonstrates coordinated movement of multiple sprites using SFML.
 * Sprites are added dynamically, follow a synchronized movement pattern, and rotate at screen edges.
 * The pattern repeats after all sprites return to the starting location and are removed.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 *
 * Author: Miles Hollifield
 * Date: 1/26/2025
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

// Screen dimensions and offsets
constexpr float SCREEN_WIDTH = 640.0f; // Width of the window
constexpr float SCREEN_HEIGHT = 480.0f; // Height of the window
constexpr float OFFSET = 20.0f; // Offset from edges to keep sprites fully visible

// Speeds
constexpr float BASE_SPEED = 200.0f; // Base speed for horizontal movement
constexpr float VERTICAL_SPEED = BASE_SPEED * (SCREEN_HEIGHT / SCREEN_WIDTH); // Adjusted speed for vertical movement

// Rotation angles
constexpr float ROTATE_RIGHT = 90.0f; // Rotation angle when moving down
constexpr float ROTATE_DOWN = 180.0f; // Rotation angle when moving left
constexpr float ROTATE_LEFT = 270.0f; // Rotation angle when moving up
constexpr float ROTATE_UP = 0.0f; // Rotation angle when moving right

// Struct to store sprite and its movement direction
struct SpriteInfo {
  sf::Sprite sprite;
  sf::Vector2f direction; // Movement direction (x, y)
  float speed; // Speed in pixels per second
  bool active = true; // Indicates if the sprite is still in motion
};

// Function to add a new sprite to the pattern
void addSprite(std::vector<SpriteInfo>& sprites, const sf::Texture& texture, sf::Vector2f startDirection, float speed) {
  SpriteInfo newSprite;
  newSprite.sprite.setTexture(texture);
  newSprite.sprite.setPosition(OFFSET, OFFSET); // Start near the top-left corner
  newSprite.direction = startDirection;
  newSprite.speed = speed;
  sprites.push_back(newSprite);
}

int main() {
  // Create the window
  sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Multiple Sprites and More Complex Movement");

  // Load the sprite texture
  sf::Texture texture;
  if (!texture.loadFromFile("boid-sm.png")) {
    std::cerr << "Failed to load sprite texture!" << std::endl;
    return -1;
  }

  // List to hold multiple sprites
  std::vector<SpriteInfo> sprites;

  // Add the first sprite
  addSprite(sprites, texture, sf::Vector2f(1.0f, 0.0f), BASE_SPEED);

  sf::Clock clock;

  // Game loop
  while (window.isOpen()) {
    // Handle events
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    // Calculate deltaTime
    float deltaTime = clock.restart().asSeconds();

    // Update sprites
    for (size_t i = 0; i < sprites.size(); ++i) {
      SpriteInfo& info = sprites[i];

      if (!info.active) continue;

      // Move the sprite based on its direction and speed
      info.sprite.move(info.direction * info.speed * deltaTime);

      // Handle edge logic
      float rightEdge = SCREEN_WIDTH - texture.getSize().x - OFFSET;
      float bottomEdge = SCREEN_HEIGHT - texture.getSize().y - OFFSET;

      if (info.direction.x > 0 && info.sprite.getPosition().x > rightEdge) { // Right edge
        info.direction = sf::Vector2f(0.0f, 1.0f); // Move down
        info.sprite.setRotation(ROTATE_RIGHT);
        info.speed = VERTICAL_SPEED; // Adjust speed for vertical movement
        if (sprites.size() == 1) addSprite(sprites, texture, sf::Vector2f(1.0f, 0.0f), BASE_SPEED); // Add second sprite
      } else if (info.direction.y > 0 && info.sprite.getPosition().y > bottomEdge) { // Bottom edge
        info.direction = sf::Vector2f(-1.0f, 0.0f); // Move left
        info.sprite.setRotation(ROTATE_DOWN);
        info.speed = BASE_SPEED; // Adjust speed for horizontal movement
        if (sprites.size() == 2) addSprite(sprites, texture, sf::Vector2f(1.0f, 0.0f), BASE_SPEED); // Add third sprite
      } else if (info.direction.x < 0 && info.sprite.getPosition().x < OFFSET) { // Left edge
        info.direction = sf::Vector2f(0.0f, -1.0f); // Move up
        info.sprite.setRotation(ROTATE_LEFT);
        info.speed = VERTICAL_SPEED; // Adjust speed for vertical movement
        if (sprites.size() == 3) addSprite(sprites, texture, sf::Vector2f(1.0f, 0.0f), BASE_SPEED); // Add fourth sprite
      } else if (info.direction.y < 0 && info.sprite.getPosition().y < OFFSET) { // Top edge
        info.direction = sf::Vector2f(1.0f, 0.0f); // Move right
        info.sprite.setRotation(ROTATE_UP);
        info.speed = BASE_SPEED; // Reset speed for horizontal movement

        // Remove the sprite if it returns to the starting position
        info.active = false;
      }
    }

    // Remove inactive sprites and reset if all are inactive
    if (std::all_of(sprites.begin(), sprites.end(), [](const SpriteInfo& info) { return !info.active; })) {
      sprites.clear();
      addSprite(sprites, texture, sf::Vector2f(1.0f, 0.0f), BASE_SPEED); // Restart with the first sprite
    }

    // Clear the window
    window.clear(sf::Color::White);

    // Draw all active sprites
    for (const auto& info : sprites) {
      if (info.active) window.draw(info.sprite);
    }

    // Display the updated frame
    window.display();
  }

  return 0;
}