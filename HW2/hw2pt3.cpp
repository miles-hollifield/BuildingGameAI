#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <deque>

// Include necessary headers
#include "headers/WanderBoid.h"

// Main Function
int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Wander Behavior");

    // Load Boid Texture
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
