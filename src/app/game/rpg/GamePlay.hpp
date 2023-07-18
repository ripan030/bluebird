#ifndef GAMEPLAY_HPP
#define GAMEPLAY_HPP

#include "State.hpp"
#include "EventMgr.hpp"

#include "Entity.hpp"
#include "MovementComponent.hpp"
#include "AnimationComponent.hpp"

class Player : public Entity {
public:
    enum State {
        IDLE,
        WALK,
    };
    Player();
    ~Player();
    void Update(const sf::Time &dt) override;
    void Render(sf::RenderTarget &target) override;
    void Move(sf::Vector2i direction, const sf::Time &dt);
    void SetState(State state);
private:
    State state;
    MovableSprite shape;
    sf::Texture textureSheet;
    MovementComponent *movementComponent;
    AnimationComponent *animationComponent;
};

struct PlayerAction {
    Player::State state;
    sf::Vector2i direction;
    void clear() {
        state = Player::State::IDLE;
        direction.x = direction.y = 0;
    }
};

class GamePlay : public State {
    Player player;
    PlayerAction action;
public:
    GamePlay(StateMgr *stateMgr);
    ~GamePlay();
    void OnCreate() override;
    void OnDestroy() override;
    void Update(const sf::Time &time) override;
    void Draw() override;
    void MainMenu(EventDetails *details);
    void GamePause(EventDetails *details);
    void MovePlayer(EventDetails *details);
};

#endif
