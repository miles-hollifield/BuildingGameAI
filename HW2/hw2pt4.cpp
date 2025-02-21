#include <SFML/Graphics.hpp>
#include "headers/FlockBoid.h"
#include <iostream>
#include <vector>

constexpr int MAX_BREADCRUMBS = 15;
constexpr int BREADCRUMB_INTERVAL = 45;
constexpr float WINDOW_WIDTH = 640;
constexpr float WINDOW_HEIGHT = 480;

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Flocking Behavior");
    sf::Texture texture;

    if (!texture.loadFromFile("boid.png")) {
        std::cerr << "Error: Failed to load boid.png" << std::endl;
        return -1;
    }

    std::vector<FlockBoid> flock;
    std::vector<std::vector<sf::CircleShape>> breadcrumbs(flock.size());
    std::vector<int> breadcrumbTimers(flock.size(), 0);

    for (int i = 0; i < 30; i++) {
        flock.emplace_back(rand() % 800, rand() % 600, texture);
        breadcrumbs.emplace_back();  
        breadcrumbTimers.emplace_back(0);
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
            breadcrumbTimers[i]++;
            if (breadcrumbTimers[i] >= BREADCRUMB_INTERVAL) {
                breadcrumbTimers[i] = 0;
                sf::CircleShape breadcrumb(3);
                breadcrumb.setFillColor(sf::Color::Blue);
                breadcrumb.setPosition(flock[i].getPosition());
                breadcrumbs[i].push_back(breadcrumb);

                if (breadcrumbs[i].size() > MAX_BREADCRUMBS) {
                    breadcrumbs[i].erase(breadcrumbs[i].begin());
                }
            }

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
