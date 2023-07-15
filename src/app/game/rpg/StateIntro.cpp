#include "StateIntro.hpp"
#include "StateMgr.hpp"

#include <iostream>

using namespace std;

IntroPlayer::IntroPlayer(const float x, const float y) :
    direction{0, 0} {
    sf::Sprite &sprite = shape.GetSprite();

    textureSheet.loadFromFile("textures/hero_spritesheet.png");

    sprite.setTexture(textureSheet);
    sprite.setPosition(x, y);

    movementComponent = new MovementComponent{shape, 0.3, 0.1, 0};
    animationComponent = new AnimationComponent;

    animationComponent->AddAnimation("HERO_WALK", sprite, textureSheet,
            {0, 94, 80, 94}, 6, 0.2);
}

IntroPlayer::~IntroPlayer() {
    delete movementComponent;
    delete animationComponent;
}

void IntroPlayer::Update(const sf::Time &dt) {
    sf::Sprite &sprite = shape.GetSprite();

    auto pos = sprite.getPosition();

    if (pos.x >= 440.0f) {
        direction.x = -1; // right
        sprite.setScale(-2.f, 2.0f);
    } else if (pos.x <= 80.0f) {
        direction.x = 1; // left
        sprite.setScale(2.f, 2.0f);
    }

    movementComponent->Move(direction.x, direction.y, dt.asSeconds());
    movementComponent->Update(dt);
    animationComponent->animations["HERO_WALK"]->Play(dt.asSeconds());
}

void IntroPlayer::Render(sf::RenderTarget &target) {
    target.draw(shape.GetSprite());
}

//------------------------------------------------------------------------

StateIntro::StateIntro(StateMgr *stateMgr) : State{stateMgr} {
    // load textures
    introTexture.loadFromFile("textures/spaceship.png");

    e = new IntroPlayer{80.f, 80.f};
}

StateIntro::~StateIntro() {
    stateMgr = nullptr;

    delete e;
}

void StateIntro::OnCreate() {
    cout << "Intro State is created\n";
    timePassed = 0.0f;

    auto windowSize = stateMgr->GetContext()->window->GetWindowSize();

    // Intialize sprites
    //heroSheet.setTexture(heroTexture);
    introSprite.setOrigin(introTexture.getSize().x / 2.0, introTexture.getSize().y / 2);

    // create sprite
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

   timePassed += time.asSeconds();

   e->Update(time);
}

void StateIntro::Draw() {
    sf::RenderWindow *window = stateMgr->GetContext()->window->GetRenderWindow();
    //window->draw(introSprite);
    if (timePassed >= textDisplay) {
        window->draw(text);
    }

    e->Render(*window);
}

void StateIntro::Continue(EventDetails *details) {
    if (timePassed >= textDisplay) {
        cout << "Continue: Move to MainMenu\n";
        stateMgr->SwitchTo(StateType::MainMenu);
    }
}
