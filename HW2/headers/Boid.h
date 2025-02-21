#ifndef BOID_H
#define BOID_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

class Boid {
public:
    Boid(float x, float y, sf::Texture& texture);

    void update(float deltaTime, const std::vector<Boid>& flock);
    void draw(sf::RenderWindow& window);
    
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getVelocity() const { return velocity; }

private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    
    sf::Sprite sprite;

    static constexpr float MAX_SPEED = 100.0f;
    static constexpr float MAX_FORCE = 5.0f;
    static constexpr float SEPARATION_RADIUS = 25.0f;
    static constexpr float ALIGNMENT_RADIUS = 50.0f;
    static constexpr float COHESION_RADIUS = 50.0f;

    sf::Vector2f separate(const std::vector<Boid>& flock);
    sf::Vector2f align(const std::vector<Boid>& flock);
    sf::Vector2f cohere(const std::vector<Boid>& flock);

    sf::Vector2f limit(sf::Vector2f vec, float max);
    sf::Vector2f normalize(sf::Vector2f vec);
};

#endif // BOID_H
