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
 
 // Constants for movement and behavior
 constexpr float MAX_SPEED = 200.0f;    // Maximum movement speed
 constexpr float ARRIVAL_RADIUS = 10.0f; // Distance threshold to stop moving
 constexpr float SLOW_RADIUS = 100.0f;   // Distance threshold to start slowing down
 constexpr float MAX_ROTATION = 5.0f;    // Max rotation step per frame
 constexpr float ALIGN_RADIUS = 5.0f;    // Threshold to determine if rotation is necessary
 constexpr int BREADCRUMB_LIMIT = 50;    // Maximum number of breadcrumbs
 constexpr int BREADCRUMB_INTERVAL = 60; // Frames between dropping breadcrumbs
 
 // Breadcrumb class to visualize movement path
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
   sf::CircleShape shape; // Small blue circle representing a breadcrumb
 };
 
 // Boid class representing the moving character
 class Boid {
 public:
   Boid(sf::RenderWindow* w, sf::Texture& tex)
       : window(w) {
     drop_timer = BREADCRUMB_INTERVAL;
     velocity = {0, 0};
     position = {300, 300}; // Start at center of the window
     target = position;
     sprite.setTexture(tex);
     sprite.setScale(0.1f, 0.1f); // Scale down sprite
     sprite.setOrigin(tex.getSize().x / 2, tex.getSize().y / 2); // Center the sprite
     sprite.setPosition(position);
   }
 
   // Update function to handle movement, alignment, and breadcrumbs
   void update(float deltaTime) {
     applyArrive(deltaTime);  // Move towards the target
     applyAlign(deltaTime);   // Rotate to face movement direction
     handleBreadcrumbs();     // Manage breadcrumbs
   }
 
   // Draw the character and its breadcrumbs
   void draw() {
     for (const auto& crumb : breadcrumbs) {
       crumb.draw(*window); // Draw each breadcrumb
     }
     window->draw(sprite); // Draw the character sprite
   }
 
   // Set a new target position when clicked
   void setTarget(sf::Vector2f newTarget) {
     target = newTarget;
   }
 
 private:
   sf::RenderWindow* window; // Pointer to the main window
   sf::Sprite sprite;        // Character sprite
   sf::Vector2f position;    // Current position
   sf::Vector2f velocity;    // Current velocity
   sf::Vector2f target;      // Target position
   float drop_timer;         // Timer for breadcrumb dropping
   std::vector<Crumb> breadcrumbs; // Stores breadcrumbs
 
   // Arrive behavior: Move towards the target with deceleration
   void applyArrive(float deltaTime) {
     sf::Vector2f desiredVelocity = target - position; // Direction to target
     float distance = std::sqrt(desiredVelocity.x * desiredVelocity.x + desiredVelocity.y * desiredVelocity.y);
 
     if (distance < ARRIVAL_RADIUS) {
       velocity = {0, 0}; // Stop moving when close enough
     } else {
       float speed = MAX_SPEED;
       if (distance < SLOW_RADIUS) {
         speed *= (distance / SLOW_RADIUS); // Gradually slow down when approaching target
       }
 
       desiredVelocity /= distance; // Normalize direction
       desiredVelocity *= speed; // Scale velocity by speed
       velocity = desiredVelocity;
     }
 
     position += velocity * deltaTime; // Update position
     sprite.setPosition(position); // Update sprite position
   }
 
   // Align behavior: Rotate to face movement direction
   void applyAlign(float deltaTime) {
     if (velocity.x == 0 && velocity.y == 0) return; // No need to rotate if not moving
 
     float targetRotation = std::atan2(velocity.y, velocity.x) * (180.0f / 3.14159265f); // Calculate desired angle
     float rotationDiff = targetRotation - sprite.getRotation(); // Difference between current and target rotation
 
     // Normalize rotation difference to avoid sudden turns over 180 degrees
     while (rotationDiff > 180.0f) rotationDiff -= 360.0f;
     while (rotationDiff < -180.0f) rotationDiff += 360.0f;
 
     if (std::abs(rotationDiff) > ALIGN_RADIUS) {
       float rotationStep = std::min(std::abs(rotationDiff), MAX_ROTATION); // Limit rotation speed
       sprite.setRotation(sprite.getRotation() + (rotationDiff > 0 ? rotationStep : -rotationStep)); // Apply rotation
     }
   }
 
   // Drop breadcrumbs to visualize movement path
   void handleBreadcrumbs() {
     if (drop_timer <= 0) {
       drop_timer = BREADCRUMB_INTERVAL; // Reset timer
       breadcrumbs.emplace_back(position); // Add a new breadcrumb
 
       if (breadcrumbs.size() > BREADCRUMB_LIMIT) {
         breadcrumbs.erase(breadcrumbs.begin()); // Remove the oldest breadcrumb
       }
     } else {
       drop_timer -= 1; // Decrease timer
     }
   }
 };
 
 int main() {
   sf::RenderWindow window(sf::VideoMode(640, 480), "Arrive and Align"); // Create game window
   sf::Texture texture;
 
   if (!texture.loadFromFile("boid.png")) { // Load character sprite
     std::cerr << "Failed to load texture!" << std::endl;
     return -1;
   }
 
   Boid boid(&window, texture); // Create boid object
   sf::Clock clock; // Clock for tracking delta time
 
   while (window.isOpen()) { // Game loop
     sf::Event event;
     while (window.pollEvent(event)) { // Event handling
       if (event.type == sf::Event::Closed)
         window.close(); // Close window if exit event triggered
       else if (event.type == sf::Event::MouseButtonPressed) {
         boid.setTarget(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window))); // Set new target on click
       }
     }
 
     float deltaTime = clock.restart().asSeconds(); // Calculate time since last frame
 
     window.clear(sf::Color::White); // Clear screen
     boid.update(deltaTime); // Update boid logic
     boid.draw(); // Render boid and breadcrumbs
     window.display(); // Display updated frame
   }
 
   return 0;
 }
 