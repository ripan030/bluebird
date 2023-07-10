#include "MainMenu.hpp"
#include "StateMgr.hpp"

#include <iostream>
using namespace std;

MainMenu::MainMenu(StateMgr *stateMgr) : State{stateMgr} {
}

MainMenu::~MainMenu() {
}

void MainMenu::OnCreate() {
    cout << "Main Menu state is created\n";

    auto windowSize = stateMgr->GetContext()->window->GetWindowSize();
    font.loadFromFile("fonts/arial.ttf");

    text.setFont(font);
    text.setString("MAIN MENU");
    text.setFillColor(sf::Color::Yellow);
    text.setCharacterSize(20);

    sf::FloatRect textRect = text.getLocalBounds();

    text.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);

    text.setPosition(windowSize.x / 2, 100);

    sf::Vector2f buttonPos = sf::Vector2f(windowSize.x / 2, 200);

    for (int i = 0; i != options.size(); ++i) {
        // update button position
        buttonPos.y += i * (buttonSize.y + 8);

        rects[i].setSize(buttonSize);
        rects[i].setFillColor(sf::Color::Red);
        rects[i].setOrigin(buttonSize.x / 2, buttonSize.y / 2);
        rects[i].setPosition(buttonPos);

        labels[i].setFont(font);
        labels[i].setString(options[i]);
        labels[i].setCharacterSize(12);

        sf::FloatRect rect = labels[i].getLocalBounds();

        labels[i].setOrigin(rect.left + rect.width / 2.0f,
                rect.top + rect.height / 2.0f);

        labels[i].setPosition(buttonPos);
    }

    EventMgr *eventMgr = stateMgr->GetContext()->eventMgr;

    eventMgr->AddCallback(StateType::MainMenu, "MouseLeft", &MainMenu::MouseClick, this);
}

void MainMenu::OnDestroy() {

}

void MainMenu::Update(const sf::Time &time) {

}

void MainMenu::Draw() {
    sf::RenderWindow *window = stateMgr->GetContext()->window->GetRenderWindow();
    window->draw(text);
    for (int i = 0; i != 2; ++i) {
        window->draw(rects[i]);
        window->draw(labels[i]);
    }
}

void MainMenu::Activate() {
    if (stateMgr->HasState(StateType::GamePlay)) {
       labels[0].setString({"Resume"});

       sf::FloatRect rect = labels[0].getLocalBounds();

       labels[0].setOrigin(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
    }
}

void MainMenu::UserInput(EventDetails *details) {
    if (details->code == sf::Keyboard::Up) {
        cout << "Continue: Move to GamePlay\n";
        stateMgr->SwitchTo(StateType::MainMenu);
    } else if (details->code == sf::Keyboard::Down) {
        cout << "Exit: Close window\n";
        stateMgr->GetContext()->window->Close();
    }
}

void MainMenu::MouseClick(EventDetails *details) {
    sf::Vector2i mouse{details->mouse};

    float halfx = buttonSize.x / 2.0f;
    float halfy = buttonSize.y / 2.0f;

    cout << "Mouse click " << mouse.x << " " << mouse.y << endl;
    for (int i = 0; i != 2; ++i) {
        if (mouse.x >= rects[i].getPosition().x - halfx &&
                mouse.x <= rects[i].getPosition().x + halfx &&
                mouse.y >= rects[i].getPosition().y - halfy &&
                mouse.y <= rects[i].getPosition().y + halfy) {

            if (i == 0) { // Play
                cout << "Play \n";
                stateMgr->SwitchTo(StateType::GamePlay);
            } else if (i == 1){
                cout << "Exit \n";
                stateMgr->GetContext()->window->Close();
            }
        }
    }
}
