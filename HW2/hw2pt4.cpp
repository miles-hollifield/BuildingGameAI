#include <SFML/Graphics.hpp>
#include "headers/Boid.h"
#include <iostream>
#include <vector>

constexpr int MAX_BREADCRUMBS = 30;  // Max trail length per boid
constexpr int BREADCRUMB_INTERVAL = 10;  // Frames between dropping breadcrumbs

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Flocking Behavior");
    sf::Texture texture;

    if (!texture.loadFromFile("boid.png")) {
        std::cerr << "Error: Failed to load boid.png" << std::endl;
        return -1;
    }

    std::vector<Boid> flock;
    std::vector<std::vector<sf::CircleShape>> breadcrumbs(flock.size()); // Breadcrumbs for each boid
    std::vector<int> breadcrumbTimers(flock.size(), 0); // Timer for each boid

    for (int i = 0; i < 30; i++) {
        flock.emplace_back(rand() % 800, rand() % 600, texture);
        breadcrumbs.emplace_back();  // Create an empty breadcrumb list for this boid
        breadcrumbTimers.emplace_back(0);  // Initialize timer for this boid
    }

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        float deltaTime = clock.restart().asSeconds();
        window.clear(sf::Color::White);

        for (size_t i = 0; i < flock.size(); i++) {
            // Drop a breadcrumb every few frames
            breadcrumbTimers[i]++;
            if (breadcrumbTimers[i] >= BREADCRUMB_INTERVAL) {
                breadcrumbTimers[i] = 0;
                sf::CircleShape breadcrumb(3);  // Small breadcrumb dot
                breadcrumb.setFillColor(sf::Color::Blue);
                breadcrumb.setPosition(flock[i].getPosition());
                breadcrumbs[i].push_back(breadcrumb);

                // Limit breadcrumb history
                if (breadcrumbs[i].size() > MAX_BREADCRUMBS) {
                    breadcrumbs[i].erase(breadcrumbs[i].begin());
                }
            }

            // Draw breadcrumbs before boid
            for (const auto& crumb : breadcrumbs[i]) {
                window.draw(crumb);
            }

            flock[i].update(deltaTime, flock);
            flock[i].draw(window);
        }

        window.display();
    }

    return 0;
}
