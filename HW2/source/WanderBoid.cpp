#include "../headers/WanderBoid.h"
#include <cmath>

// Breadcrumb class constructor
Crumb::Crumb(sf::Vector2f pos) {
    shape.setRadius(3.0f); // Set radius for the crumb
    shape.setFillColor(sf::Color::Blue); // Set color for the crumb
    shape.setPosition(pos); // Set position for the crumb
}

// Draws the crumb
void Crumb::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

// WanderBoid constructor
WanderBoid::WanderBoid(sf::RenderWindow* w, sf::Texture& tex, std::vector<Crumb>* crumbs)
    : window(w), breadcrumbs(crumbs), rng(rd()), angleChangeDist(-WANDER_ANGLE_SMOOTHING, WANDER_ANGLE_SMOOTHING) {
    position = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2}; // Initialize position
    velocity = {MAX_SPEED, 0}; // Initialize velocity
    wanderAngle = 0; // Initialize wander angle
    orientation = 0; // Initialize orientation
    sprite.setTexture(tex); // Set texture for the sprite
    sprite.setScale(0.1f, 0.1f); // Set scale for the sprite
    sprite.setOrigin(tex.getSize().x / 2, tex.getSize().y / 2); // Set origin for the sprite
    sprite.setPosition(position); // Set initial position for the sprite
}

// Updates the boid's position and state
void WanderBoid::update(float deltaTime) {
    applyWander(deltaTime); // Apply wander behavior
    handleBoundaries(); // Handle screen boundaries
    dropBreadcrumbs(); // Drop breadcrumbs
}

// Draws the boid
void WanderBoid::draw() {
    window->draw(sprite);
}

// Applies the wander behavior to determine the boid's movement direction
void WanderBoid::applyWander(float deltaTime) {
    // Calculate the center of the wander circle ahead of the boid
    sf::Vector2f circleCenter = position + normalize(velocity) * WANDER_CIRCLE_DISTANCE;

    // Gradually adjust the wander angle with randomness
    wanderAngle += angleChangeDist(rng);

    // Calculate the displacement vector based on the adjusted wander angle
    float angleRad = wanderAngle * (3.14159265f / 180.0f);
    sf::Vector2f displacement(WANDER_CIRCLE_RADIUS * std::cos(angleRad), WANDER_CIRCLE_RADIUS * std::sin(angleRad));

    // Determine the new target velocity based on the wander target position
    sf::Vector2f wanderTarget = circleCenter + displacement;
    velocity = normalize(wanderTarget - position) * MAX_SPEED;

    // Update position based on velocity
    position += velocity * deltaTime;

    // Update orientation of the boid to face the new direction
    updateOrientation();

    // Update the position and rotation of the sprite
    sprite.setPosition(position);
    sprite.setRotation(orientation);
}

// Normalizes a vector to unit length
sf::Vector2f WanderBoid::normalize(sf::Vector2f vec) {
    float magnitude = std::sqrt(vec.x * vec.x + vec.y * vec.y);
    return (magnitude != 0) ? sf::Vector2f(vec.x / magnitude, vec.y / magnitude) : sf::Vector2f(0, 0);
}

// Handle screen boundaries by wrapping around the edges
void WanderBoid::handleBoundaries() {
    if (position.x < 0) position.x = WINDOW_WIDTH;
    if (position.x > WINDOW_WIDTH) position.x = 0;
    if (position.y < 0) position.y = WINDOW_HEIGHT;
    if (position.y > WINDOW_HEIGHT) position.y = 0;
}

// Drop Breadcrumbs
void WanderBoid::dropBreadcrumbs() {
    static int dropTimer = BREADCRUMB_INTERVAL;
    if (--dropTimer <= 0) {
        breadcrumbs->push_back(Crumb(position)); // Drop a breadcrumb at the current position
        dropTimer = BREADCRUMB_INTERVAL;
        if (breadcrumbs->size() > BREADCRUMB_LIMIT) {
            breadcrumbs->erase(breadcrumbs->begin()); // Remove the oldest breadcrumb if limit exceeded
        }
    }
}

/** OpenAI's ChatGPT was used to suggest different methods for changing the boid's orientation. 
 * The following prompt was used: "Give me suggestions for different methods of changing the boid's 
 * orientation to match its movement direction."
 */

// Updates the boid's orientation to face its movement direction
void WanderBoid::updateOrientation() {
    float targetOrientation = std::atan2(velocity.y, velocity.x) * (180.0f / 3.14159265f);

    // METHOD 1: Instant Snap to Direction
    orientation = targetOrientation;

    // METHOD 2: Smooth Rotation (Uncomment this line to use)
    // orientation += (targetOrientation - orientation) * ROTATION_SMOOTH_FACTOR;
}

/** End ChatGPT citation */
