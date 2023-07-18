#include "GamePlay.hpp"
#include "EventMgr.hpp"
#include "StateMgr.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;

Player::Player() {
    state = IDLE;
    sf::Sprite &sprite = shape.GetSprite();
    sf::IntRect rect{0, 94, 80, 94};

    textureSheet.loadFromFile("textures/hero_spritesheet.png");

    sprite.setTexture(textureSheet);
    sprite.setTexture(textureSheet, true);
    sprite.setTextureRect(rect);
    sprite.setPosition(50, 50);

    sprite.setScale(1.0f, 1.0f);

    movementComponent = new MovementComponent{shape, 120, 100, 35};
    animationComponent = new AnimationComponent;

    animationComponent->AddAnimation("HERO_IDLE", sprite, textureSheet,
            rect, 1, 0);

    animationComponent->AddAnimation("HERO_WALK", sprite, textureSheet,
            rect, 6, 0.1);
}

Player::~Player() {
    delete movementComponent;
    delete animationComponent;
}

void Player::Update(const sf::Time &dt) {
    movementComponent->Update(dt);

    if (state == WALK) {
        bool done = animationComponent->animations["HERO_WALK"]->Play(dt.asSeconds());
        if (done && movementComponent->GetVelocity().x == 0) {
            state = IDLE;
        }
    }

    if (state == IDLE) {
        animationComponent->animations["HERO_IDLE"]->Play(dt.asSeconds());
    }
}

void Player::Render(sf::RenderTarget &target) {
    target.draw(shape.GetSprite());
}

void Player::Move(sf::Vector2i direction, const sf::Time &dt) {
    sf::Sprite &sprite = shape.GetSprite();

    if (direction.x > 0) {
        sprite.setScale(1.0f, 1.0f);
    } else if (direction.x < 0) {
        sprite.setScale(-1.0f, 1.0f);
    }

    movementComponent->Move(direction.x, direction.y, dt.asSeconds());
}

void Player::SetState(State state) {
    this->state = state;
}

GamePlay::GamePlay(StateMgr *stateMgr) : State{stateMgr} {
}

GamePlay::~GamePlay() {
}

void GamePlay::OnCreate() {
    cout << "Game Play \n";

    EventMgr *eventMgr = stateMgr->GetContext()->eventMgr;

    eventMgr->AddCallback(StateType::GamePlay, "PressedEsc", &GamePlay::MainMenu, this);
    eventMgr->AddCallback(StateType::GamePlay, "PressedP", &GamePlay::GamePause, this);
    eventMgr->AddCallback(StateType::GamePlay, "PressedA", &GamePlay::MovePlayer, this);
    eventMgr->AddCallback(StateType::GamePlay, "PressedD", &GamePlay::MovePlayer, this);
    eventMgr->AddCallback(StateType::GamePlay, "ReleasedD", &GamePlay::MovePlayer, this);
}

void GamePlay::OnDestroy() {
}

void GamePlay::Update(const sf::Time &dt) {
    if (action.state != Player::State::IDLE) {
        player.Move(action.direction, dt);
        player.SetState(action.state);
    }

    action.clear();

    player.Update(dt);
}

void GamePlay::Draw() {
    sf::RenderWindow *window = stateMgr->GetContext()->window->GetRenderWindow();

    player.Render(*window);
}

void GamePlay::MainMenu(EventDetails *details) {
    stateMgr->SwitchTo(StateType::MainMenu);
}

void GamePlay::GamePause(EventDetails *details) {
    stateMgr->SwitchTo(StateType::GamePaused);
}

void GamePlay::MovePlayer(EventDetails *details) {
    switch (details->code) {
    case sf::Keyboard::A:
        if (details->type == EventType::KeyDown) {
            action.direction.x = -1;
            action.state = Player::State::WALK;
        }
        break;
    case sf::Keyboard::D:
        if (details->type == EventType::KeyDown) {
            action.direction.x = 1;
            action.state = Player::State::WALK;
        }
        break;
    default:
        return;
    }
}
