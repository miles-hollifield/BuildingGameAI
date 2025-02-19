/**
 * Part 3: Wander Steering Behavior
 *
 * This program demonstrates a wander algorithm using SFML.
 * - The shape moves randomly while maintaining smooth directional changes.
 * - Two different orientation methods are implemented for comparison.
 * - Breadcrumbs visualize the movement pattern.
 * - The shape handles screen boundaries gracefully.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 *
 * Author: Miles Hollifield
 * Date: 1/28/2025
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <random>

// Constants
constexpr float MAX_SPEED = 150.0f;  // Maximum movement speed
constexpr float WANDER_CIRCLE_DISTANCE = 50.0f; // Distance for wander calculations
constexpr float WANDER_CIRCLE_RADIUS = 20.0f;  // Radius of the wander circle
constexpr float WANDER_ANGLE_SMOOTHING = 3.0f; // Smaller = smoother changes
constexpr int BREADCRUMB_LIMIT = 30; // Max breadcrumbs
constexpr int BREADCRUMB_INTERVAL = 60; // Frames between dropping breadcrumbs
constexpr float SCREEN_WIDTH = 640;
constexpr float SCREEN_HEIGHT = 480;

// Random generator
std::random_device rd;
std::mt19937 rng(rd());
std::uniform_real_distribution<float> angleChangeDist(-WANDER_ANGLE_SMOOTHING, WANDER_ANGLE_SMOOTHING);

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

// Wander Boid class
class WanderBoid {
public:
  WanderBoid(sf::RenderWindow* w, sf::Texture& tex, std::vector<Crumb>* crumbs)
      : window(w), breadcrumbs(crumbs) {
    position = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}; // Start at center
    velocity = {MAX_SPEED, 0};  // Initially moving right
    wanderAngle = 0;
    sprite.setTexture(tex);
    sprite.setScale(0.1f, 0.1f);
    sprite.setOrigin(tex.getSize().x / 2, tex.getSize().y / 2);
    sprite.setPosition(position);
  }

  void update(float deltaTime) {
    applyWander(deltaTime);
    handleBoundaries();
    dropBreadcrumbs();
  }

  void draw() {
    window->draw(sprite);
  }

private:
  sf::RenderWindow* window;
  sf::Sprite sprite;
  sf::Vector2f position;
  sf::Vector2f velocity;
  float wanderAngle;
  std::vector<Crumb>* breadcrumbs;

  // Wander Algorithm - Smooth Circle-Based Approach
  void applyWander(float deltaTime) {
    // Calculate wander target ahead of the boid
    sf::Vector2f circleCenter = position + normalize(velocity) * WANDER_CIRCLE_DISTANCE;

    // Adjust the wander angle gradually
    wanderAngle += angleChangeDist(rng);

    // Compute displacement based on adjusted wander angle
    float angleRad = wanderAngle * (3.14159265f / 180.0f);
    sf::Vector2f displacement(WANDER_CIRCLE_RADIUS * std::cos(angleRad), WANDER_CIRCLE_RADIUS * std::sin(angleRad));

    // Set new velocity direction towards the wander target
    sf::Vector2f wanderTarget = circleCenter + displacement;
    velocity = normalize(wanderTarget - position) * MAX_SPEED;

    // Update position and sprite
    position += velocity * deltaTime;
    sprite.setPosition(position);
    sprite.setRotation(std::atan2(velocity.y, velocity.x) * (180.0f / 3.14159265f));
  }

  // Normalize a vector
  sf::Vector2f normalize(sf::Vector2f vec) {
    float magnitude = std::sqrt(vec.x * vec.x + vec.y * vec.y);
    return (magnitude != 0) ? sf::Vector2f(vec.x / magnitude, vec.y / magnitude) : sf::Vector2f(0, 0);
  }

  // Handle screen boundaries by wrapping around
  void handleBoundaries() {
    if (position.x < 0) position.x = SCREEN_WIDTH;
    if (position.x > SCREEN_WIDTH) position.x = 0;
    if (position.y < 0) position.y = SCREEN_HEIGHT;
    if (position.y > SCREEN_HEIGHT) position.y = 0;
  }

  // Drop Breadcrumbs
  void dropBreadcrumbs() {
    static int dropTimer = BREADCRUMB_INTERVAL;
    if (--dropTimer <= 0) {
      breadcrumbs->push_back(Crumb(position));
      dropTimer = BREADCRUMB_INTERVAL;
      if (breadcrumbs->size() > BREADCRUMB_LIMIT) {
        breadcrumbs->erase(breadcrumbs->begin());
      }
    }
  }
};

int main() {
  sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Wander Behavior");
  sf::Texture texture;

  if (!texture.loadFromFile("boid.png")) {
    std::cerr << "Failed to load texture!" << std::endl;
    return -1;
  }

  std::vector<Crumb> breadcrumbs;
  WanderBoid boid(&window, texture, &breadcrumbs);

  sf::Clock clock;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    float deltaTime = clock.restart().asSeconds();

    window.clear(sf::Color::White);
    boid.update(deltaTime);
    for (const auto& crumb : breadcrumbs) crumb.draw(window);
    boid.draw();
    window.display();
  }

  return 0;
}
