/**
 * @file hw2pt3.cpp
 * @brief Implements a simple wander behavior for an autonomous boid using SFML.
 *
 * Resources Used:
 * - SFML Official Tutorials: https://www.sfml-dev.org/learn.php
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - File: Provided Breadcrumb class from TA Derek Martin
 * - AI Tools: OpenAI's ChatGPT
 *
 * Author: Miles Hollifield
 * Date: 2/23/2025
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <deque>

// Include necessary headers
#include "headers/WanderBoid.h"

// Main Function
int main() {
    // Create SFML window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Part 3: Wander Steering Behaviors");

    // Load Boid Texture
    sf::Texture texture;
    if (!texture.loadFromFile("boid.png")) {
        std::cerr << "Failed to load texture!" << std::endl;
        return -1;
    }

    // Setup a vector to store breadcrumbs
    std::vector<Crumb> breadcrumbs;

    // Create a WanderBoid object to exhibit wandering behavior
    WanderBoid boid(&window, texture, &breadcrumbs);

    // Clock to manage delta time
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close(); // Close window
        }

        // Get delta time
        float deltaTime = clock.restart().asSeconds();

        // Render
        window.clear(sf::Color::White);

        // Update and draw wandering boid and breadcrumbs
        boid.update(deltaTime);
        for (const auto& crumb : breadcrumbs) crumb.draw(window);
        boid.draw();
        window.display();
    }

    return 0;
}
