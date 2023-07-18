#ifndef MOVEMENT_COMPONENT_HPP
#define MOVEMENT_COMPONENT_HPP

#include <SFML/Graphics.hpp>

class MovableShape {
public:
    MovableShape() {};
    virtual ~MovableShape() {};
    virtual void Move(const float offset_x, const float offset_y) = 0;
    virtual void Move(const sf::Vector2f offset) = 0;
};

class MovableSprite : public MovableShape {
    sf::Sprite sprite;
public:
    void Move(const float offset_x, const float offset_y) override {
        sprite.move(offset_x, offset_y);
    }
    void Move(const sf::Vector2f offset) override {
        sprite.move(offset);
    }
    sf::Sprite &GetSprite() {
        return sprite;
    }
};

class MovementComponent {
    MovableShape &shape;
    const float maxSpeed;
    const float acceleration;
    const float deceleration;
    sf::Vector2f velocity;
public:
    MovementComponent(MovableShape &shape, const float maxSpeed, const float acceleration, const float deceleration);
    ~MovementComponent();
    void Move(const float x, const float y, const float dt);
    void Update(const sf::Time &dt);
    sf::Vector2f GetVelocity() const {
        return velocity;
    }
};

#endif
