#include "../headers/FlockBoid.h"

// Constructor
FlockBoid::FlockBoid(float x, float y, sf::Texture& texture) {
    position = sf::Vector2f(x, y); // Set initial position
    velocity = sf::Vector2f((rand() % 200 - 100) / 100.0f, (rand() % 200 - 100) / 100.0f); // Random initial velocity
    velocity = normalize(velocity) * MAX_SPEED; // Limit initial velocity to max speed
    acceleration = sf::Vector2f(0, 0); // Initialize acceleration

    sprite.setTexture(texture); // Set texture for the sprite
    sprite.setScale(0.03f, 0.03f); // Set scale for the sprite
    sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f); // Set origin for the sprite
}

// Update function updates the state of the FlockBoid based on flock behaviors
void FlockBoid::update(float deltaTime, const std::vector<FlockBoid>& flock) {
    sf::Vector2f sep = separate(flock) * 2.0f; // Separation behavior
    sf::Vector2f ali = align(flock) * 0.9f; // Alignment behavior
    sf::Vector2f coh = cohere(flock) * 0.8f; // Cohesion behavior

    // Combine all behavior forces to calculate final acceleration
    acceleration += sep + ali + coh;

    velocity += acceleration; // Update velocity based on acceleration
    velocity = limit(velocity, MAX_SPEED); // Limit velocity to max speed
    position += velocity * deltaTime; // Update position based on velocity

    acceleration = sf::Vector2f(0, 0); // Reset acceleration each frame

    // Wrap around screen edges
    if (position.x < 0) position.x = 800;
    if (position.x > 800) position.x = 0;
    if (position.y < 0) position.y = 600;
    if (position.y > 600) position.y = 0;

    sprite.setPosition(position); // Update sprite position
    sprite.setRotation(atan2(velocity.y, velocity.x) * 180.0f / 3.14159265f); // Update sprite rotation
}

// Draw function
void FlockBoid::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// Separation behavior calculates a force to keep boids apart
sf::Vector2f FlockBoid::separate(const std::vector<FlockBoid>& flock) {
    sf::Vector2f steer(0, 0);
    int count = 0;
    
    // Iterate through flock to calculate separation force
    for (const auto& other : flock) {
        float dist = hypot(other.position.x - position.x, other.position.y - position.y); // Calculate distance
        if (dist > 0 && dist < SEPARATION_RADIUS) { // If within separation range
            sf::Vector2f diff = position - other.position; // Vector pointing away from the other boid
            float weight = (SEPARATION_RADIUS - dist) / SEPARATION_RADIUS; // Weighted influence
            diff = normalize(diff) * weight; // Apply weighted repulsion
            steer += diff * 3.0f; // Increase separation strength
            count++; // Count the number of boids influencing the separation
        }
    }
    
    if (count > 0) steer /= static_cast<float>(count); // Average the steering force 
    if (hypot(steer.x, steer.y) > 0) steer = normalize(steer) * MAX_FORCE; // Limit steering force

    // Return the steering force
    return steer;
}

// Alignment behavior calculates a force to align boids with neighbors
sf::Vector2f FlockBoid::align(const std::vector<FlockBoid>& flock) {
    sf::Vector2f avgVelocity(0, 0);
    int count = 0;
    
    // Iterate through flock to calculate alignment force
    for (const auto& other : flock) {
        float dist = hypot(other.position.x - position.x, other.position.y - position.y); // Calculate distance
        if (dist > 0 && dist < ALIGNMENT_RADIUS) { // If within alignment range
            avgVelocity += other.velocity; // Sum velocities of neighbors
            count++; // Count the number of boids influencing the alignment
        }
    }
    
    if (count > 0) {
        avgVelocity /= static_cast<float>(count); // Average the velocities
        avgVelocity = normalize(avgVelocity) * (MAX_SPEED * 0.8f); // Limit to max speed
        sf::Vector2f steer = avgVelocity - velocity; // Calculate steering force
        return limit(steer, MAX_FORCE * 0.7f); // Limit steering force
    }
    
    // If no neighbors, return zero steering force
    return sf::Vector2f(0, 0);
}

// Cohesion behavior calculates a force to move boids toward the center of mass
sf::Vector2f FlockBoid::cohere(const std::vector<FlockBoid>& flock) {
    sf::Vector2f centerMass(0, 0);
    int count = 0;
    
    // Iterate through flock to calculate cohesion force
    for (const auto& other : flock) {
        float dist = hypot(other.position.x - position.x, other.position.y - position.y); // Calculate distance
        if (dist > 0 && dist < COHESION_RADIUS) { // If within cohesion range
            centerMass += other.position; // Sum positions of neighbors
            count++; // Count the number of boids influencing the cohesion
        }
    }
    
    if (count > 0) {
        centerMass /= static_cast<float>(count); // Average the positions
        sf::Vector2f steer = normalize(centerMass - position) * (MAX_FORCE * 0.6f); // Calculate steering force
        return steer; // Return steering force
    }
    
    // If no neighbors, return zero steering force
    return sf::Vector2f(0, 0);
}

// Utility function to limit the magnitude of a vector
sf::Vector2f FlockBoid::limit(sf::Vector2f vec, float max) {
    float mag = hypot(vec.x, vec.y);
    if (mag > max) return normalize(vec) * max; // Scale down to max magnitude
    return vec;
}

// Utility function to normalize a vector to unit length
sf::Vector2f FlockBoid::normalize(sf::Vector2f vec) {
    float mag = hypot(vec.x, vec.y);
    return (mag > 0) ? vec / mag : vec;
}
