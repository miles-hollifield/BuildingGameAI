/**
 * @file PathFollower.h
 * @brief Defines the PathFollower class for following paths via steering behaviors.
 *
 * Author: Miles Hollifield
 * Date: 3/20/2025
 */

 #ifndef PATHFOLLOWER_H
 #define PATHFOLLOWER_H
 
 #include <SFML/Graphics.hpp>
 #include <vector>
 #include <deque>
 #include "headers/Arrive.h"
 #include "headers/Align.h"
 #include "headers/Kinematic.h"
 
 /**
  * @class Breadcrumb
  * @brief Visual representation of past positions for tracking movement.
  */
 class Breadcrumb {
 public:
     /**
      * @brief Constructor for a breadcrumb at the given position.
      * @param pos Position of the breadcrumb.
      */
     Breadcrumb(sf::Vector2f pos) {
         shape.setRadius(3.0f);
         shape.setFillColor(sf::Color::Blue);
         shape.setPosition(pos);
         shape.setOrigin(3.0f, 3.0f);
     }
     
     /**
      * @brief Draw the breadcrumb on the window.
      * @param window The window to draw to.
      */
     void draw(sf::RenderWindow& window) const {
         window.draw(shape);
     }
     
 private:
     sf::CircleShape shape;
 };
 
 /**
  * @class PathFollower
  * @brief Agent that follows paths using steering behaviors.
  */
 class PathFollower {
 public:
     /**
      * @brief Constructor for creating a path follower.
      * @param startPosition Initial position of the agent.
      * @param texture Texture for the agent sprite.
      */
     PathFollower(sf::Vector2f startPosition, sf::Texture& texture) 
         : arriveBehavior(250.0f, 175.0f, 5.0f, 120.0f, 0.2f),
           alignBehavior(15.0f, 200.0f, 1.0f, 40.0f, 0.05f),
           breadcrumbCounter(0),
           currentWaypoint(0) {
         
         // Initialize kinematic state
         character.position = startPosition;
         character.velocity = {0, 0};
         character.orientation = 0;
         character.rotation = 0;
         
         // Initialize target
         target.position = startPosition;
         target.velocity = {0, 0};
         target.orientation = 0;
         target.rotation = 0;
         
         // Initialize sprite
         sprite.setTexture(texture);
         sprite.setScale(0.1f, 0.1f);
         sprite.setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);
         sprite.setPosition(startPosition);
     }
     
     /**
      * @brief Set a new path for the agent to follow.
      * @param waypoints Vector of points defining the path.
      */
     void setPath(const std::vector<sf::Vector2f>& waypoints) {
         if (waypoints.empty()) {
             return;
         }
         
         path = waypoints;
         currentWaypoint = 0;
         
         // Set initial target to first waypoint
         if (!path.empty()) {
             target.position = path[currentWaypoint];
         }
         
         // Clear breadcrumbs
         breadcrumbs.clear();
     }
     
     /**
      * @brief Update the agent's position to follow the path.
      * @param deltaTime Time since last update.
      */
     void update(float deltaTime) {
         // If no path or at the end of the path, don't move
         if (path.empty() || currentWaypoint >= path.size()) {
             return;
         }
         
         // Calculate desired orientation based on current velocity
         if (character.velocity.x != 0 || character.velocity.y != 0) {
             float angle = std::atan2(character.velocity.y, character.velocity.x);
             target.orientation = angle * (180.0f / 3.14159265f);
         }
         
         // Apply Arrive behavior to navigate to current waypoint
         SteeringData arriveAcceleration = arriveBehavior.calculateAcceleration(character, target);
         character.velocity += arriveAcceleration.linear * deltaTime;
         
         // Apply Align behavior to match orientation
         SteeringData alignAcceleration = alignBehavior.calculateAcceleration(character, target);
         character.rotation += alignAcceleration.angular * deltaTime;
         
         // Update character position and orientation
         character.update(deltaTime);
         
         // Check if we've reached the current waypoint
         float distToWaypoint = std::sqrt(
             (target.position.x - character.position.x) * (target.position.x - character.position.x) +
             (target.position.y - character.position.y) * (target.position.y - character.position.y)
         );
         
         if (distToWaypoint < 15.0f) {  // Threshold for reaching a waypoint
             currentWaypoint++;
             
             // If there are more waypoints, set the next one as target
             if (currentWaypoint < path.size()) {
                 target.position = path[currentWaypoint];
             }
         }
         
         // Update sprite position and rotation
         sprite.setPosition(character.position);
         sprite.setRotation(character.orientation);
         
         // Handle breadcrumbs
         dropBreadcrumbs();
     }
     
     /**
      * @brief Check if the agent has reached the end of the path.
      * @return True if the path is completed.
      */
     bool pathCompleted() const {
         return path.empty() || currentWaypoint >= path.size();
     }
     
     /**
      * @brief Get the current position of the agent.
      * @return Current position.
      */
     sf::Vector2f getPosition() const {
         return character.position;
     }
     
     /**
      * @brief Draw the agent and its path on the window.
      * @param window The window to draw to.
      */
     void draw(sf::RenderWindow& window) {
         // Draw breadcrumbs first
         for (const auto& crumb : breadcrumbs) {
             crumb.draw(window);
         }
         
         // Draw the path
         if (path.size() > 1) {
             sf::VertexArray lines(sf::LinesStrip, path.size());
             for (size_t i = 0; i < path.size(); i++) {
                 lines[i].position = path[i];
                 lines[i].color = sf::Color(0, 150, 0, 150);
             }
             window.draw(lines);
             
             // Draw waypoints as small circles
             for (const auto& waypoint : path) {
                 sf::CircleShape waypointMarker(5);
                 waypointMarker.setFillColor(sf::Color(0, 100, 0));
                 waypointMarker.setOrigin(5, 5);
                 waypointMarker.setPosition(waypoint);
                 window.draw(waypointMarker);
             }
         }
         
         // Draw the sprite
         window.draw(sprite);
     }
     
 private:
     Kinematic character;
     Kinematic target;
     sf::Sprite sprite;
     Arrive arriveBehavior;
     Align alignBehavior;
     
     std::vector<sf::Vector2f> path;
     int currentWaypoint;
     
     std::deque<Breadcrumb> breadcrumbs;
     int breadcrumbCounter;
     static constexpr int BREADCRUMB_INTERVAL = 20;  // Frames between dropping breadcrumbs
     static constexpr int MAX_BREADCRUMBS = 50;      // Maximum number of breadcrumbs
     
     /**
      * @brief Drop breadcrumbs to visualize the agent's path.
      */
     void dropBreadcrumbs() {
         breadcrumbCounter++;
         if (breadcrumbCounter >= BREADCRUMB_INTERVAL) {
             breadcrumbCounter = 0;
             breadcrumbs.push_back(Breadcrumb(character.position));
             
             if (breadcrumbs.size() > MAX_BREADCRUMBS) {
                 breadcrumbs.pop_front();
             }
         }
     }
 };
 
 #endif // PATHFOLLOWER_H