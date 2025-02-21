#include "../headers/FlockBoid.h"

// Constructor
FlockBoid::FlockBoid(float x, float y, sf::Texture& texture) {
    position = sf::Vector2f(x, y);
    velocity = sf::Vector2f((rand() % 200 - 100) / 100.0f, (rand() % 200 - 100) / 100.0f);
    velocity = normalize(velocity) * MAX_SPEED;
    acceleration = sf::Vector2f(0, 0);

    sprite.setTexture(texture);
    sprite.setScale(0.03f, 0.03f);
    sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
}

// Update function
void FlockBoid::update(float deltaTime, const std::vector<FlockBoid>& flock) {
    sf::Vector2f sep = separate(flock) * 2.0f;  
    sf::Vector2f ali = align(flock) * 0.9f;     
    sf::Vector2f coh = cohere(flock) * 0.8f;    

    acceleration += sep + ali + coh;

    velocity += acceleration;
    velocity = limit(velocity, MAX_SPEED);
    position += velocity * deltaTime;

    acceleration = sf::Vector2f(0, 0); // Reset acceleration each frame

    // Wrap around screen edges
    if (position.x < 0) position.x = 800;
    if (position.x > 800) position.x = 0;
    if (position.y < 0) position.y = 600;
    if (position.y > 600) position.y = 0;

    sprite.setPosition(position);
    sprite.setRotation(atan2(velocity.y, velocity.x) * 180.0f / 3.14159265f);
}

// Draw function
void FlockBoid::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// Separation Behavior
sf::Vector2f FlockBoid::separate(const std::vector<FlockBoid>& flock) {
    sf::Vector2f steer(0, 0);
    int count = 0;
    
    for (const auto& other : flock) {
        float dist = hypot(other.position.x - position.x, other.position.y - position.y);
        if (dist > 0 && dist < SEPARATION_RADIUS) { 
            sf::Vector2f diff = position - other.position;
            float weight = (SEPARATION_RADIUS - dist) / SEPARATION_RADIUS;  
            diff = normalize(diff) * weight;  
            steer += diff * 3.0f;  
            count++;
        }
    }
    
    if (count > 0) steer /= static_cast<float>(count); 
    if (hypot(steer.x, steer.y) > 0) steer = normalize(steer) * MAX_FORCE; 
    return steer;
}

// Alignment Behavior
sf::Vector2f FlockBoid::align(const std::vector<FlockBoid>& flock) {
    sf::Vector2f avgVelocity(0, 0);
    int count = 0;
    
    for (const auto& other : flock) {
        float dist = hypot(other.position.x - position.x, other.position.y - position.y);
        if (dist > 0 && dist < ALIGNMENT_RADIUS) { 
            avgVelocity += other.velocity;
            count++;
        }
    }
    
    if (count > 0) {
        avgVelocity /= static_cast<float>(count); 
        avgVelocity = normalize(avgVelocity) * (MAX_SPEED * 0.8f);
        sf::Vector2f steer = avgVelocity - velocity;
        return limit(steer, MAX_FORCE * 0.7f);
    }
    
    return sf::Vector2f(0, 0);
}

// Cohesion Behavior
sf::Vector2f FlockBoid::cohere(const std::vector<FlockBoid>& flock) {
    sf::Vector2f centerMass(0, 0);
    int count = 0;
    
    for (const auto& other : flock) {
        float dist = hypot(other.position.x - position.x, other.position.y - position.y);
        if (dist > 0 && dist < COHESION_RADIUS) { 
            centerMass += other.position;
            count++;
        }
    }
    
    if (count > 0) {
        centerMass /= static_cast<float>(count);
        sf::Vector2f steer = normalize(centerMass - position) * (MAX_FORCE * 0.6f);
        return steer;
    }
    
    return sf::Vector2f(0, 0);
}

// Utility Functions
sf::Vector2f FlockBoid::limit(sf::Vector2f vec, float max) {
    float mag = hypot(vec.x, vec.y);
    if (mag > max) return normalize(vec) * max;
    return vec;
}

sf::Vector2f FlockBoid::normalize(sf::Vector2f vec) {
    float mag = hypot(vec.x, vec.y);
    return (mag > 0) ? vec / mag : vec;
}
