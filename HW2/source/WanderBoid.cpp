#include "../headers/WanderBoid.h"
#include <cmath>

// Breadcrumb class constructor
Crumb::Crumb(sf::Vector2f pos) {
    shape.setRadius(3.0f);
    shape.setFillColor(sf::Color::Blue);
    shape.setPosition(pos);
}

void Crumb::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

// WanderBoid constructor
WanderBoid::WanderBoid(sf::RenderWindow* w, sf::Texture& tex, std::vector<Crumb>* crumbs)
    : window(w), breadcrumbs(crumbs), rng(rd()), angleChangeDist(-WANDER_ANGLE_SMOOTHING, WANDER_ANGLE_SMOOTHING) {
    position = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}; 
    velocity = {MAX_SPEED, 0};  
    wanderAngle = 0;
    orientation = 0;
    sprite.setTexture(tex);
    sprite.setScale(0.1f, 0.1f);
    sprite.setOrigin(tex.getSize().x / 2, tex.getSize().y / 2);
    sprite.setPosition(position);
}

// Update function
void WanderBoid::update(float deltaTime) {
    applyWander(deltaTime);
    handleBoundaries();
    dropBreadcrumbs();
}

// Draw function
void WanderBoid::draw() {
    window->draw(sprite);
}

// Wander Algorithm - Smooth Circle-Based Approach
void WanderBoid::applyWander(float deltaTime) {
    // Calculate wander target ahead of the boid
    sf::Vector2f circleCenter = position + normalize(velocity) * WANDER_CIRCLE_DISTANCE;

    // Adjust the wander angle gradually
    wanderAngle += angleChangeDist(rng);

    // Compute displacement based on adjusted wander angle
    float angleRad = wanderAngle * (3.14159265f / 180.0f);
    sf::Vector2f displacement(WANDER_CIRCLE_RADIUS * std::cos(angleRad), WANDER_CIRCLE_RADIUS * std::sin(angleRad));

    // Set new velocity direction towards the wander target
    sf::Vector2f wanderTarget = circleCenter + displacement;
    velocity = normalize(wanderTarget - position) * MAX_SPEED;

    // Update position
    position += velocity * deltaTime;

    // Update orientation based on chosen method
    updateOrientation();

    // Update sprite
    sprite.setPosition(position);
    sprite.setRotation(orientation);
}

// Normalize a vector
sf::Vector2f WanderBoid::normalize(sf::Vector2f vec) {
    float magnitude = std::sqrt(vec.x * vec.x + vec.y * vec.y);
    return (magnitude != 0) ? sf::Vector2f(vec.x / magnitude, vec.y / magnitude) : sf::Vector2f(0, 0);
}

// Handle screen boundaries by wrapping around
void WanderBoid::handleBoundaries() {
    if (position.x < 0) position.x = SCREEN_WIDTH;
    if (position.x > SCREEN_WIDTH) position.x = 0;
    if (position.y < 0) position.y = SCREEN_HEIGHT;
    if (position.y > SCREEN_HEIGHT) position.y = 0;
}

// Drop Breadcrumbs
void WanderBoid::dropBreadcrumbs() {
    static int dropTimer = BREADCRUMB_INTERVAL;
    if (--dropTimer <= 0) {
        breadcrumbs->push_back(Crumb(position));
        dropTimer = BREADCRUMB_INTERVAL;
        if (breadcrumbs->size() > BREADCRUMB_LIMIT) {
            breadcrumbs->erase(breadcrumbs->begin());
        }
    }
}

// Update Orientation - Choose ONE method
void WanderBoid::updateOrientation() {
    float targetOrientation = std::atan2(velocity.y, velocity.x) * (180.0f / 3.14159265f);

    // METHOD 1: Instant Snap to Direction
    orientation = targetOrientation;

    // METHOD 2: Smooth Rotation (Uncomment this line to use)
    // orientation += (targetOrientation - orientation) * ROTATION_SMOOTH_FACTOR;
}
