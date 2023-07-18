#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>

class Entity {
protected:
public:
    Entity() {}
    virtual ~Entity() {}
    virtual void Update(const sf::Time &dt) = 0;
    virtual void Render(sf::RenderTarget &target) = 0;
};

#endif
