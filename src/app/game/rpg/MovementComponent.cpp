#include "MovementComponent.hpp"

#include <iostream>

using namespace std;

MovementComponent::MovementComponent(MovableShape &shape, const float maxSpeed, const float acceleration, const float deceleration) :
    shape{shape}, maxSpeed{maxSpeed}, acceleration{acceleration}, deceleration{deceleration} {

    velocity.x = velocity.y = 0;
}

MovementComponent::~MovementComponent() {
}

void MovementComponent::Move(const float dir_x, const float dir_y, const float dt) {
    velocity.x += acceleration * dir_x * dt;
    velocity.y += acceleration * dir_y * dt;
}

void MovementComponent::Update(const sf::Time &dt) {
    if (velocity.x > 0.f) {
        if (velocity.x > maxSpeed) {
            velocity.x = maxSpeed;
        }

        velocity.x -= deceleration * dt.asSeconds();
        if (velocity.x < 0.f) {
            velocity.x = 0.f;
        }
    } else if (velocity.x < 0.f) {
        if (velocity.x < -maxSpeed) {
            velocity.x = -maxSpeed;
        }

        velocity.x += deceleration * dt.asSeconds();
        if (velocity.x > 0.f) {
            velocity.x = 0.f;
        }
    }

    shape.Move(velocity * dt.asSeconds());
}

