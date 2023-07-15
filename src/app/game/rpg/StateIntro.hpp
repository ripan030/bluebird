#ifndef STATEINTRO_HPP
#define STATEINTRO_HPP

#include "State.hpp"
#include "EventMgr.hpp"

#include "Entity.hpp"
#include "MovementComponent.hpp"
#include "AnimationComponent.hpp"

class IntroPlayer : public Entity {
    MovableSprite shape;
    sf::Texture textureSheet;
    sf::Vector2f direction;
    MovementComponent *movementComponent;
    AnimationComponent *animationComponent;
public:
    IntroPlayer(const float pos_x, const float pos_y);
    ~IntroPlayer();
    void Update(const sf::Time &dt) override;
    void Render(sf::RenderTarget &target) override;
};

class StateIntro : public State {
    float timePassed;
    float rotationAngle;
    sf::Text text;
    sf::Texture introTexture;
    sf::Sprite introSprite;
    sf::Font font;
    const float textDisplay = 2.0f;

    sf::Sprite heroSheet;
    sf::Texture heroTexture;

public:
    IntroPlayer *e;
    StateIntro(StateMgr *stateMgr);
    ~StateIntro();
    void OnCreate() override;
    void OnDestroy() override;
    void Update(const sf::Time &time) override;
    void Draw() override;
    void Continue(EventDetails *details);
};

#endif
