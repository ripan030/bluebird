#include "GamePlay.hpp"
#include "EventMgr.hpp"
#include "StateMgr.hpp"

#include <iostream>

using namespace std;

GamePlay::GamePlay(StateMgr *stateMgr) : State{stateMgr} {
}

GamePlay::~GamePlay() {
}

void GamePlay::OnCreate() {
    cout << "Game Play \n";

    rotationAngle = 0;
    // Draw a sprite

    auto windowSize = stateMgr->GetContext()->window->GetWindowSize();

    playTexture.loadFromFile("textures/spaceship.png");
    playSprite.setTexture(playTexture);
    playSprite.setScale(0.5, 0.5);
    playSprite.setOrigin(playTexture.getSize().x / 2.0, playTexture.getSize().y / 2);
    playSprite.setPosition(windowSize.x / 2, windowSize.y / 2);


    EventMgr *eventMgr = stateMgr->GetContext()->eventMgr;


    eventMgr->AddCallback(StateType::GamePlay, "PressedEsc", &GamePlay::MainMenu, this);
    eventMgr->AddCallback(StateType::GamePlay, "PressedP", &GamePlay::GamePause, this);
}

void GamePlay::OnDestroy() {
}

void GamePlay::Update(const sf::Time &time) {
    rotationAngle += 0.5;
    if (rotationAngle > 360)
        rotationAngle = 0;
    playSprite.setRotation(rotationAngle);
}

void GamePlay::Draw() {
    sf::RenderWindow *window = stateMgr->GetContext()->window->GetRenderWindow();

    window->draw(playSprite);
}

void GamePlay::MainMenu(EventDetails *details) {
    stateMgr->SwitchTo(StateType::MainMenu);
}

void GamePlay::GamePause(EventDetails *details) {
    stateMgr->SwitchTo(StateType::GamePaused);
}
