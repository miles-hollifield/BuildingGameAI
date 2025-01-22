#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

struct SpriteInfo {
  sf::Sprite sprite;
  sf::Vector2f direction; // Movement direction
};

int main() {
  // Create the window
  sf::RenderWindow window(sf::VideoMode(640, 480), "Multiple Sprites");

  // Load the sprite texture
  sf::Texture texture;
  if (!texture.loadFromFile("boid-sm.png")) {
    std::cout << "Failed to load texture!" << std::endl;
    return -1;
  }

  // List to hold multiple sprites
  std::vector<SpriteInfo> sprites;

  // Add the first sprite
  SpriteInfo firstSprite;
  firstSprite.sprite.setTexture(texture);
  firstSprite.sprite.setPosition(0.f, 50.f); // Top-left
  firstSprite.direction = sf::Vector2f(200.f, 0.f); // Move right
  sprites.push_back(firstSprite);

  sf::Clock clock;

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
    for (auto &info : sprites) {
      // Move sprite
      info.sprite.move(info.direction * deltaTime);

      // Handle edge collisions and rotations
      if (info.direction.x > 0 && info.sprite.getPosition().x > 640 - texture.getSize().x) { // Right edge
        info.direction = sf::Vector2f(0.f, 200.f); // Move down
        info.sprite.setRotation(90.f);
      } else if (info.direction.y > 0 && info.sprite.getPosition().y > 480 - texture.getSize().y) { // Bottom edge
        info.direction = sf::Vector2f(-200.f, 0.f); // Move left
        info.sprite.setRotation(180.f);
      } else if (info.direction.x < 0 && info.sprite.getPosition().x < 0) { // Left edge
        info.direction = sf::Vector2f(0.f, -200.f); // Move up
        info.sprite.setRotation(270.f);
      } else if (info.direction.y < 0 && info.sprite.getPosition().y < 0) { // Top edge
        info.direction = sf::Vector2f(200.f, 0.f); // Move right
        info.sprite.setRotation(0.f);
      }
    }

    // Add a new sprite at intervals
    if (sprites.size() < 4 && sprites.back().sprite.getPosition().x > 300) {
      SpriteInfo newSprite;
      newSprite.sprite.setTexture(texture);
      newSprite.sprite.setPosition(0.f, 50.f);
      newSprite.direction = sf::Vector2f(200.f, 0.f); // Start moving right
      sprites.push_back(newSprite);
    }

    // Clear and draw everything
    window.clear(sf::Color::White);
    for (const auto &info : sprites) {
      window.draw(info.sprite);
    }
    window.display();
  }

  return 0;
}
