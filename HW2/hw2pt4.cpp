/**
 * Part 4: Flocking Behavior (Boids Algorithm)
 *
 * Implements Reynolds' Boids algorithm:
 * - Separation: Avoids collisions with nearby boids.
 * - Alignment: Matches velocity with nearby boids.
 * - Cohesion: Moves toward the center of mass of nearby boids.
 * - Breadcrumbs visualize motion.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Reynolds' Boids Algorithm (1987)
 *
 * Author: Miles Hollifield
 * Date: 1/30/2025
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <random>

// Constants
constexpr int BOID_COUNT = 20;
constexpr float MAX_SPEED = 100.0f; // Lowered for more natural motion
constexpr float SEPARATION_RADIUS = 80.0f; // Increased to avoid clustering
constexpr float NEIGHBOR_RADIUS = 100.0f;
constexpr float SEPARATION_FORCE = 2.5f; // Increased to push apart more
constexpr float ALIGNMENT_FORCE = 0.8f; // Reduced to prevent excessive grouping
constexpr float COHESION_FORCE = 0.5f; // Reduced to lessen attraction to group center
constexpr float SCREEN_WIDTH = 640;
constexpr float SCREEN_HEIGHT = 480;
constexpr int BREADCRUMB_LIMIT = 40;
constexpr int BREADCRUMB_INTERVAL = 10;  // Keep breadcrumbs spread out

// Random generator
std::random_device rd;
std::mt19937 rng(rd());
std::uniform_real_distribution<float> randPos(50.0f, SCREEN_WIDTH - 50.0f);
std::uniform_real_distribution<float> randVel(-MAX_SPEED, MAX_SPEED);

// Helper Functions
sf::Vector2f normalize(sf::Vector2f v) {
  float magnitude = std::sqrt(v.x * v.x + v.y * v.y);
  return (magnitude > 0) ? v / magnitude : sf::Vector2f(0, 0);
}

float distance(sf::Vector2f a, sf::Vector2f b) {
  return std::sqrt(std::pow(b.x - a.x, 2) + std::pow(b.y - a.y, 2));
}

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
  Boid(sf::RenderWindow* w, sf::Texture& tex, std::vector<Boid>* allBoids)
      : window(w), boids(allBoids), drop_timer(BREADCRUMB_INTERVAL) {
    position = {randPos(rng), randPos(rng)};
    velocity = {randVel(rng), randVel(rng)};
    sprite.setTexture(tex);
    sprite.setScale(0.1f, 0.1f);
    sprite.setOrigin(tex.getSize().x / 2, tex.getSize().y / 2);
    sprite.setPosition(position);
  }

  void update(float deltaTime) {
    applyFlocking();
    velocity = normalize(velocity) * MAX_SPEED;
    position += velocity * deltaTime;
    handleBoundaries();
    sprite.setPosition(position);
    sprite.setRotation(std::atan2(velocity.y, velocity.x) * (180.0f / 3.14159265f));
    dropBreadcrumbs();
  }

  void draw(sf::RenderWindow& window) {
    window.draw(sprite);
    for (auto& crumb : breadcrumbs) crumb.draw(window);
  }

private:
  sf::RenderWindow* window;
  sf::Sprite sprite;
  sf::Vector2f position;
  sf::Vector2f velocity;
  std::vector<Boid>* boids;
  std::vector<Crumb> breadcrumbs;
  int drop_timer;

  void applyFlocking() {
    sf::Vector2f separation(0, 0);
    sf::Vector2f alignment(0, 0);
    sf::Vector2f cohesion(0, 0);
    int separationCount = 0, alignmentCount = 0, cohesionCount = 0;

    for (const auto& boid : *boids) {
      if (&boid == this) continue;
      float dist = distance(position, boid.position);

      // Separation: Repel strongly when too close
      if (dist < SEPARATION_RADIUS) {
        sf::Vector2f diff = normalize(position - boid.position);
        separation += diff * (SEPARATION_RADIUS / (dist + 0.1f)); // Avoid divide by zero
        separationCount++;
      }

      // Alignment: Steer to match velocity of nearby boids
      if (dist < NEIGHBOR_RADIUS) {
        alignment += boid.velocity;
        alignmentCount++;
      }

      // Cohesion: Move toward center of mass of nearby boids
      if (dist < NEIGHBOR_RADIUS) {
        cohesion += boid.position;
        cohesionCount++;
      }
    }

    if (separationCount > 0) separation = normalize(separation / static_cast<float>(separationCount)) * SEPARATION_FORCE;
    if (alignmentCount > 0) alignment = normalize(alignment / static_cast<float>(alignmentCount)) * ALIGNMENT_FORCE;
    if (cohesionCount > 0) cohesion = normalize((cohesion / static_cast<float>(cohesionCount)) - position) * COHESION_FORCE;

    velocity += separation + alignment + cohesion;

    // Cap max speed dynamically
    if (std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y) > MAX_SPEED) {
      velocity = normalize(velocity) * MAX_SPEED;
    }
  }

  void handleBoundaries() {
    if (position.x < 0) position.x = SCREEN_WIDTH;
    if (position.x > SCREEN_WIDTH) position.x = 0;
    if (position.y < 0) position.y = SCREEN_HEIGHT;
    if (position.y > SCREEN_HEIGHT) position.y = 0;
  }

  void dropBreadcrumbs() {
    if (--drop_timer <= 0) {
      breadcrumbs.push_back(Crumb(position));
      drop_timer = BREADCRUMB_INTERVAL;
      if (breadcrumbs.size() > BREADCRUMB_LIMIT) {
        breadcrumbs.erase(breadcrumbs.begin());
      }
    }
  }
};

int main() {
  sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Flocking Behavior");
  sf::Texture texture;

  if (!texture.loadFromFile("boid.png")) {
    std::cerr << "Failed to load texture!" << std::endl;
    return -1;
  }

  std::vector<Boid> boids;
  for (int i = 0; i < BOID_COUNT; i++) {
    boids.emplace_back(&window, texture, &boids);
  }

  sf::Clock clock;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    float deltaTime = clock.restart().asSeconds();

    window.clear(sf::Color::White);
    for (auto& boid : boids) boid.update(deltaTime);
    for (auto& boid : boids) boid.draw(window);
    window.display();
  }

  return 0;
}
