#include "StateIntro.hpp"
#include "StateMgr.hpp"

#include <iostream>

using namespace std;

StateIntro::StateIntro(StateMgr *stateMgr) : State{stateMgr} {
}

StateIntro::~StateIntro() {
    stateMgr = nullptr;
}

void StateIntro::OnCreate() {
    cout << "Intro State is created\n";
    timePassed = 0.0f;

    auto windowSize = stateMgr->GetContext()->window->GetWindowSize();

    // create sprite
    introTexture.loadFromFile("textures/spaceship.png");
    introSprite.setTexture(introTexture);
    introSprite.setScale(0.5, 0.5);
    introSprite.setOrigin(introTexture.getSize().x / 2.0, introTexture.getSize().y / 2);
    introSprite.setPosition(windowSize.x / 2, windowSize.y / 2);

    rotationAngle = 0;

    font.loadFromFile("fonts/arial.ttf");

    text.setFont(font);
    text.setString("Press SPACE to continue");
    text.setFillColor(sf::Color::Yellow);
    text.setCharacterSize(20);

    sf::FloatRect textRect = text.getLocalBounds();

    text.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);

    text.setPosition(windowSize.x / 2, windowSize.y - textRect.height - 8);

    EventMgr *eventMgr = stateMgr->GetContext()->eventMgr;
    eventMgr->AddCallback(StateType::Intro, {"PressedSpace"}, &StateIntro::Continue, this);
}

void StateIntro::OnDestroy() {

}

void StateIntro::Update(const sf::Time &time) {
   rotationAngle += 0.5;
   if (rotationAngle > 360)
       rotationAngle = 0;
   introSprite.setRotation(rotationAngle);

   if (timePassed < 600.0f) {
       timePassed += time.asSeconds();
   }
}

void StateIntro::Draw() {
    sf::RenderWindow *window = stateMgr->GetContext()->window->GetRenderWindow();
    window->draw(introSprite);
    if (timePassed > 600.0f) {
        window->draw(text);
    }
}

void StateIntro::Continue(EventDetails *details) {
    if (timePassed >= 600.0f) {
        cout << "Continue: Move to MainMenu\n";
        stateMgr->SwitchTo(StateType::MainMenu);
    }
}
