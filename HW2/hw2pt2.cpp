/**
 * Part 2: Arrive and Align
 *
 * This program demonstrates the Arrive and Align behaviors using SFML.
 * - The character moves toward the mouse click location using Arrive behavior.
 * - The character smoothly rotates to face its movement direction using Align behavior.
 * - Breadcrumbs visualize the movement pattern.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 *
 * Author: Miles Hollifield
 * Date: 1/27/2025
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>

// Constants
constexpr float MAX_SPEED = 200.0f;   // Maximum movement speed
constexpr float ARRIVAL_RADIUS = 10.0f; // Stop moving when within this range
constexpr float SLOW_RADIUS = 100.0f; // Slow down approaching target
constexpr float MAX_ROTATION = 5.0f; // Max rotation step per frame
constexpr float ALIGN_RADIUS = 5.0f; // Rotation threshold
constexpr int BREADCRUMB_LIMIT = 50; // Max breadcrumbs stored
constexpr int BREADCRUMB_INTERVAL = 60; // Frames between dropping breadcrumbs

// Breadcrumb class
class Crumb {
public:
  Crumb(sf::Vector2f pos) {
    shape.setRadius(3.0f);
    shape.setFillColor(sf::Color::Blue);
    shape.setPosition(pos);
  }

  void draw(sf::RenderWindow& window) const {
    window.draw(shape);
  }

private:
  sf::CircleShape shape;
};

// Boid class
class Boid {
public:
  Boid(sf::RenderWindow* w, sf::Texture& tex)
      : window(w) {
    drop_timer = BREADCRUMB_INTERVAL;
    velocity = {0, 0};
    position = {300, 300}; // Start at center
    target = position;
    sprite.setTexture(tex);
    sprite.setScale(0.1f, 0.1f);
    sprite.setOrigin(tex.getSize().x / 2, tex.getSize().y / 2);
    sprite.setPosition(position);
  }

  void update(float deltaTime) {
    applyArrive(deltaTime);
    applyAlign(deltaTime);
    handleBreadcrumbs();
  }

  void draw() {
    for (const auto& crumb : breadcrumbs) {
      crumb.draw(*window);
    }
    window->draw(sprite);
  }

  void setTarget(sf::Vector2f newTarget) {
    target = newTarget;
  }

private:
  sf::RenderWindow* window;
  sf::Sprite sprite;
  sf::Vector2f position;
  sf::Vector2f velocity;
  sf::Vector2f target;
  float drop_timer;
  std::vector<Crumb> breadcrumbs;

  void applyArrive(float deltaTime) {
    sf::Vector2f desiredVelocity = target - position;
    float distance = std::sqrt(desiredVelocity.x * desiredVelocity.x + desiredVelocity.y * desiredVelocity.y);

    if (distance < ARRIVAL_RADIUS) {
      velocity = {0, 0}; // Stop moving
    } else {
      float speed = MAX_SPEED;
      if (distance < SLOW_RADIUS) {
        speed *= (distance / SLOW_RADIUS); // Slow down as it approaches target
      }

      desiredVelocity /= distance; // Normalize
      desiredVelocity *= speed;
      velocity = desiredVelocity;
    }

    position += velocity * deltaTime;
    sprite.setPosition(position);
  }

  void applyAlign(float deltaTime) {
    if (velocity.x == 0 && velocity.y == 0) return; // No need to rotate if not moving

    float targetRotation = std::atan2(velocity.y, velocity.x) * (180.0f / 3.14159265f);
    float rotationDiff = targetRotation - sprite.getRotation();

    while (rotationDiff > 180.0f) rotationDiff -= 360.0f;
    while (rotationDiff < -180.0f) rotationDiff += 360.0f;

    if (std::abs(rotationDiff) > ALIGN_RADIUS) {
      float rotationStep = std::min(std::abs(rotationDiff), MAX_ROTATION);
      sprite.setRotation(sprite.getRotation() + (rotationDiff > 0 ? rotationStep : -rotationStep));
    }
  }

  void handleBreadcrumbs() {
    if (drop_timer <= 0) {
      drop_timer = BREADCRUMB_INTERVAL;
      breadcrumbs.emplace_back(position); // Add new breadcrumb

      if (breadcrumbs.size() > BREADCRUMB_LIMIT) {
        breadcrumbs.erase(breadcrumbs.begin()); // Remove oldest breadcrumb
      }
    } else {
      drop_timer -= 1;
    }
  }
};

int main() {
  sf::RenderWindow window(sf::VideoMode(600, 600), "Arrive and Align");
  sf::Texture texture;

  if (!texture.loadFromFile("boid.png")) {
    std::cerr << "Failed to load texture!" << std::endl;
    return -1;
  }

  Boid boid(&window, texture);
  sf::Clock clock;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      else if (event.type == sf::Event::MouseButtonPressed) {
        boid.setTarget(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
      }
    }

    float deltaTime = clock.restart().asSeconds();

    window.clear(sf::Color::White);
    boid.update(deltaTime);
    boid.draw();
    window.display();
  }

  return 0;
}
