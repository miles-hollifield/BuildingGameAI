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
constexpr float WINDOW_WIDTH = 600;
constexpr float WINDOW_HEIGHT = 600;
constexpr float SPRITE_SCALE = 0.1f; // Adjust scale for boid.png
constexpr int BREADCRUMB_LIMIT = 50; // Max breadcrumbs stored
constexpr int BREADCRUMB_INTERVAL = 60; // Frames between dropping breadcrumbs

// Breadcrumb class for visualizing motion history
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

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Arrive & Align Behavior");

    // Load Boid Texture
    sf::Texture boidTexture;
    if (!boidTexture.loadFromFile("boid.png")) {
        std::cerr << "Failed to load boid.png!" << std::endl;
        return -1;
    }

    // Character setup (using sprite)
    sf::Sprite character;
    character.setTexture(boidTexture);
    character.setScale(SPRITE_SCALE, SPRITE_SCALE);
    character.setOrigin(boidTexture.getSize().x / 2, boidTexture.getSize().y / 2); // Center sprite
    sf::Vector2f characterPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    // Kinematic Objects
    Kinematic characterKinematic(characterPosition, {0, 0}, 0, 0);
    Kinematic targetKinematic(characterPosition, {0, 0}, 0, 0); // No movement initially

    // Steering Behaviors (Fine-tuned parameters for better control)
    Arrive arriveBehavior(200.0f, 150.0f, 10.0f, 100.0f, 0.1f);
    Align alignBehavior(10.0f, 180.0f, 2.0f, 30.0f, 0.1f);

    sf::Clock clock;

    // Breadcrumb storage
    std::deque<Crumb> breadcrumbs;
    int breadcrumbCounter = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
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

        // **Ensure stopping at target**
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

        // Update SFML Sprite
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
            crumb.draw(window);
        }
        window.draw(character);
        window.display();
    }

    return 0;
}
