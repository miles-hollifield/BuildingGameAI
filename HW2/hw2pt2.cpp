/**
 * @file hw2pt2.cpp
 * @brief Implements the Arrive and Align steering behaviors with breadcrumb visualization and mouse clicks.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - File: Provided Breadcrumb class from TA Derek Martin
 *
 * Author: Miles Hollifield
 * Date: 2/23/2025
 */

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cmath>
#include <deque>

// Include necessary headers
#include "headers/Kinematic.h"
#include "headers/Arrive.h"
#include "headers/Align.h"

// Constants
constexpr float WINDOW_WIDTH = 640; // Window width
constexpr float WINDOW_HEIGHT = 480; // Window height
constexpr float SPRITE_SCALE = 0.1f; // Scale adjustment for boid character
constexpr int BREADCRUMB_LIMIT = 50; // Max breadcrumbs stored
constexpr int BREADCRUMB_INTERVAL = 60; // Frames between dropping breadcrumbs

// Breadcrumb class for visualizing motion history
class Crumb {
public:
    // Constructor
    Crumb(sf::Vector2f pos) {
        shape.setRadius(3.0f);
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(pos);
    }

    // Draw crumb
    void draw(sf::RenderWindow& window) const {
        window.draw(shape);
    }

private:
    sf::CircleShape shape; // Circle shape representing the breadcrumb
};

int main() {
    // Create an SFML window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Part 2: Arrive and Align");

    // Load Boid Texture
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

    // Initialize kinematic objects
    Kinematic characterKinematic(characterPosition, {0, 0}, 0, 0);
    Kinematic targetKinematic(characterPosition, {0, 0}, 0, 0); // No movement initially

    // Initialize steering behaviors

    /* METHOD 1:
     * Smoother, gradual approach and alignment:
     * - Arrive: max acceleration 200, max speed 150, target radius 10, slow radius 100, timeToTarget 0.1
     * - Align: max angular acceleration 10, max rotation 180, target radius 2, slow radius 30
     */
    // Arrive arriveBehavior(200.0f, 150.0f, 10.0f, 100.0f, 0.1f);
    // Align alignBehavior(10.0f, 180.0f, 2.0f, 30.0f, 0.1f);

    /* METHOD 2 (ALTERNATE - Commented Out):
     * Faster approach with quick deceleration:
     * - Arrive: max acceleration 250, max speed 175, target radius 5, slow radius 120, timeToTarget 0.2
     * - Align: max angular acceleration 15, max rotation 200, target radius 1, slow radius 40
     */
    Arrive arriveBehavior(250.0f, 175.0f, 5.0f, 120.0f, 0.2f);
    Align alignBehavior(15.0f, 200.0f, 1.0f, 40.0f, 0.05f);

    sf::Clock clock;

    // Breadcrumb storage
    std::deque<Crumb> breadcrumbs;
    int breadcrumbCounter = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close(); // Close window
            else if (event.type == sf::Event::MouseButtonPressed) {
                // Set the target position when the mouse is clicked
                targetKinematic.position = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
            }
        }

        // Get delta time
        float deltaTime = clock.restart().asSeconds();

        // Apply Arrive behavior
        SteeringData arriveAcceleration = arriveBehavior.calculateAcceleration(characterKinematic, targetKinematic);
        characterKinematic.velocity += arriveAcceleration.linear * deltaTime;

        // Make sure character stops near target
        float speed = std::sqrt(characterKinematic.velocity.x * characterKinematic.velocity.x + 
                                characterKinematic.velocity.y * characterKinematic.velocity.y);

        if (speed < 0.5f) {
            characterKinematic.velocity = {0, 0};  // Stop completely if close enough
        }

        characterKinematic.update(deltaTime);

        // Apply Align behavior
        SteeringData alignAcceleration = alignBehavior.calculateAcceleration(characterKinematic, targetKinematic);
        characterKinematic.rotation += alignAcceleration.angular * deltaTime;
        characterKinematic.update(deltaTime);

        // Update the sprite position
        character.setPosition(characterKinematic.position);

        // Set sprite rotation based on movement direction
        if (characterKinematic.velocity.x != 0 || characterKinematic.velocity.y != 0) {
            float angle = std::atan2(characterKinematic.velocity.y, characterKinematic.velocity.x) * (180.0f / 3.14159265f);
            character.setRotation(angle);
        }

        // Handle Breadcrumbs
        if (--breadcrumbCounter <= 0) {
            breadcrumbs.emplace_back(characterKinematic.position);
            breadcrumbCounter = BREADCRUMB_INTERVAL;

            if (breadcrumbs.size() > BREADCRUMB_LIMIT) {
                breadcrumbs.pop_front(); // Remove oldest breadcrumb
            }
        }

        // Render
        window.clear(sf::Color::White);
        for (const auto& crumb : breadcrumbs) {
            crumb.draw(window); // Draw each breadcrumb
        }
        window.draw(character); // Draw the character sprite
        window.display();
    }

    return 0;
}
