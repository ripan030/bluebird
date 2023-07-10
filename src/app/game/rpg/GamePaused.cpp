#include "GamePaused.hpp"
#include "EventMgr.hpp"
#include "StateMgr.hpp"

#include <iostream>

using namespace std;

GamePaused::GamePaused(StateMgr *stateMgr) : State{stateMgr} {
}

GamePaused::~GamePaused() {
}

void GamePaused::Unpause(EventDetails *details) {
    stateMgr->SwitchTo(StateType::GamePlay);
}

void GamePaused::OnCreate() {
    cout << "Game Paused\n";

    SetTransparent(true);

    auto windowSize = stateMgr->GetContext()->window->GetWindowSize();

    font.loadFromFile("fonts/arial.ttf");

    text.setFont(font);
    text.setString({"PAUSED"});
    text.setCharacterSize(15);
    text.setStyle(sf::Text::Style::Bold);

    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
    text.setPosition(windowSize.x / 2, windowSize.y / 2);

    rect.setSize(sf::Vector2f{windowSize});
    rect.setPosition(0, 0);
    rect.setFillColor(sf::Color{0, 0, 0, 150});

    EventMgr *eventMgr = stateMgr->GetContext()->eventMgr;
    eventMgr->AddCallback(StateType::GamePaused, "PressedP", &GamePaused::Unpause, this);

}

void GamePaused::OnDestroy() {
}

void GamePaused::Update(const sf::Time &time) {
}

void GamePaused::Draw() {
    sf::RenderWindow *window = stateMgr->GetContext()->window->GetRenderWindow();

    window->draw(rect);
    window->draw(text);
}
