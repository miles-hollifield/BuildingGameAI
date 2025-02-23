/**
 * @file hw2pt4.cpp
 * @brief Implements flocking behavior using the Boids algorithm with SFML.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 *
 * Author: Miles Hollifield
 * Date: 2/23/2025
 */

#include <SFML/Graphics.hpp>
#include "headers/FlockBoid.h"
#include <iostream>
#include <vector>

constexpr int MAX_BREADCRUMBS = 15; // Max trail length for each boid
constexpr int BREADCRUMB_INTERVAL = 45; // Frames between dropping breadcrumbs
constexpr float WINDOW_WIDTH = 640; // Window width
constexpr float WINDOW_HEIGHT = 480; // Window height

int main() {
    // Create SFML window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Part 4: Flocking Behavior and Blending/Arbitration");

    // Load Boid texture
    sf::Texture texture;
    if (!texture.loadFromFile("boid.png")) {
        std::cerr << "Error: Failed to load boid.png" << std::endl;
        return -1;
    }

    // Create a vector to store the flock of boids
    std::vector<FlockBoid> flock;
    // Vectors to store breadcrumbs and timers for each boid
    std::vector<std::vector<sf::CircleShape>> breadcrumbs(flock.size());
    std::vector<int> breadcrumbTimers(flock.size(), 0);

    // Initialize the flock with 30 boids
    for (int i = 0; i < 30; i++) {
        flock.emplace_back(rand() % 800, rand() % 600, texture);
        breadcrumbs.emplace_back(); // Create an empty breadcrumb list for each boid
        breadcrumbTimers.emplace_back(0); // Initialize timer for each boid
    }

    // Clock to manage delta time
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        // Get delta time
        float deltaTime = clock.restart().asSeconds();

        // Render
        window.clear(sf::Color::White);

        // Iterate over flock
        for (size_t i = 0; i < flock.size(); i++) {
            // Drop a breadcrumb every few frames
            breadcrumbTimers[i]++;
            if (breadcrumbTimers[i] >= BREADCRUMB_INTERVAL) {
                breadcrumbTimers[i] = 0;
                sf::CircleShape breadcrumb(3); // Small breadcrumb dot
                breadcrumb.setFillColor(sf::Color::Blue);
                breadcrumb.setPosition(flock[i].getPosition());
                breadcrumbs[i].push_back(breadcrumb);

                // Limit breadcrumb trail length
                if (breadcrumbs[i].size() > MAX_BREADCRUMBS) {
                    breadcrumbs[i].erase(breadcrumbs[i].begin()); // Remove oldest breadcrumb
                }
            }

            // Draw breadcrumbs before rendering the boid
            for (const auto& crumb : breadcrumbs[i]) {
                window.draw(crumb);
            }

            // Update and render each boid
            flock[i].update(deltaTime, flock);
            flock[i].draw(window);
        }

        // Display frame
        window.display();
    }

    return 0;
}
