#ifndef WANDERBOID_H
#define WANDERBOID_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

// Constants
constexpr float MAX_SPEED = 150.0f;
constexpr float WANDER_CIRCLE_DISTANCE = 50.0f;
constexpr float WANDER_CIRCLE_RADIUS = 20.0f;
constexpr float WANDER_ANGLE_SMOOTHING = 3.0f;
constexpr float ROTATION_SMOOTH_FACTOR = 0.05f;
constexpr int BREADCRUMB_LIMIT = 30;
constexpr int BREADCRUMB_INTERVAL = 60;
constexpr float WINDOW_WIDTH = 640;
constexpr float WINDOW_HEIGHT = 480;

// Breadcrumb class
class Crumb {
public:
    Crumb(sf::Vector2f pos);
    void draw(sf::RenderWindow& window) const;
private:
    sf::CircleShape shape;
};

// WanderBoid class
class WanderBoid {
public:
    WanderBoid(sf::RenderWindow* w, sf::Texture& tex, std::vector<Crumb>* crumbs);
    void update(float deltaTime);
    void draw();

private:
    sf::RenderWindow* window;
    sf::Sprite sprite;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float wanderAngle;
    float orientation;
    std::vector<Crumb>* breadcrumbs;

    std::random_device rd;
    std::mt19937 rng;
    std::uniform_real_distribution<float> angleChangeDist;

    void applyWander(float deltaTime);
    sf::Vector2f normalize(sf::Vector2f vec);
    void handleBoundaries();
    void dropBreadcrumbs();
    void updateOrientation();
};

#endif // WANDERBOID_H
