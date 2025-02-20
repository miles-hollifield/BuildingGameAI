#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cmath>
#include <deque>

// Include necessary headers
#include "headers/Kinematic.h"
#include "headers/PositionMatching.h"
#include "headers/OrientationMatching.h"

// Constants
constexpr float WINDOW_WIDTH = 640;
constexpr float WINDOW_HEIGHT = 480;
constexpr float SPRITE_SCALE = 0.1f;  // Adjust scale for boid.png
constexpr int BREADCRUMB_LIMIT = 30;  // Max breadcrumbs stored
constexpr int BREADCRUMB_INTERVAL = 5; // Frames between dropping breadcrumbs

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
    Kinematic targetKinematic(characterPosition, {0, 0}, 0, 0);  // Start with no target movement

    // Steering Behaviors
    PositionMatching positionMatching(200.0f, 150.0f, 5.0f, 75.0f, 0.1f);
    OrientationMatching orientationMatching(10.0f, 180.0f, 1.0f, 20.0f, 0.1f);

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

        // Apply Arrive (Position Matching)
        SteeringData positionAccel = positionMatching.calculateAcceleration(characterKinematic, targetKinematic);
        characterKinematic.velocity += positionAccel.linear * deltaTime;

        // Ensure stopping at target
        if (std::sqrt(positionAccel.linear.x * positionAccel.linear.x + positionAccel.linear.y * positionAccel.linear.y) < 0.5f &&
            std::sqrt(characterKinematic.velocity.x * characterKinematic.velocity.x + characterKinematic.velocity.y * characterKinematic.velocity.y) < 1.0f) {
            characterKinematic.velocity = {0, 0};  // Stop completely if close enough
        }

        characterKinematic.update(deltaTime);

        // Apply Align (Orientation Matching)
        SteeringData orientationAccel = orientationMatching.calculateAcceleration(characterKinematic, targetKinematic);
        characterKinematic.rotation += orientationAccel.angular * deltaTime;
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
                breadcrumbs.pop_front();  // Remove oldest breadcrumb
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
