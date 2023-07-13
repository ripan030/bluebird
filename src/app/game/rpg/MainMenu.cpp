#include "MainMenu.hpp"
#include "StateMgr.hpp"

#include <iostream>
using namespace std;

void MainMenu::InitGui() {
    font.loadFromFile("fonts/arial.ttf");

    // Configure Window background
    bgWindow.setSize(sf::Vector2f{640, 480});
    bgWindow.setFillColor(sf::Color{128, 128, 128, 128});

    // Configure Menu background
    bgMenu.setSize(sf::Vector2f{100 + 2 * 5, 400});
    bgMenu.setPosition(sf::Vector2f{100 - 5, 40});
    bgMenu.setFillColor(sf::Color(10, 10, 10, 220));
    //bgMenu.setOutlineThickness(1.0f);
    //bgMenu.setOutlineColor(sf::Color::Red);

    // Add buttons
    buttons["Play"] = new Button{100, 100, 100, 25, "Play", 20, &font};
    buttonActions["Play"] = [this]() {
                stateMgr->SwitchTo(StateType::GamePlay);
    };
    buttons["Quit"] = new Button{100, 150, 100, 25, "Quit", 20, &font};
    buttonActions["Quit"] = [this]() {
                stateMgr->GetContext()->window->Close();
    };
}

MainMenu::MainMenu(StateMgr *stateMgr) : State{stateMgr} {
    InitGui();
}

MainMenu::~MainMenu() {
    for (auto it = buttons.begin(); it != buttons.end(); ++it) {
        delete it->second;
    }
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

    EventMgr *eventMgr = stateMgr->GetContext()->eventMgr;

    eventMgr->AddCallback(StateType::MainMenu, "MouseLeft", &MainMenu::MouseClick, this);
}

void MainMenu::OnDestroy() {

}

void MainMenu::UpdateButtons() {
    sf::RenderWindow *window = stateMgr->GetContext()->window->GetRenderWindow();
    for (auto &it : buttons) {
        if (it.second->GetState() == ButtonState::BUTTON_ACTIVE) {
            it.second->Update(sf::Mouse::getPosition(*window));
            it.second->SetState(ButtonState::BUTTON_IDLE);
            continue;
        }
        auto rect = it.second->GetRect();
        if (rect.contains(sf::Vector2f{sf::Mouse::getPosition(*window)})) {
            it.second->SetState(ButtonState::BUTTON_HOVER);
        } else {
            it.second->SetState(ButtonState::BUTTON_IDLE);
        }
        it.second->Update(sf::Mouse::getPosition(*window));
    }
}

void MainMenu::Update(const sf::Time &time) {
    UpdateButtons();
}

void MainMenu::RenderButtons(sf::RenderTarget &target) {
    for (auto &it : buttons) {
        it.second->Render(target);
    }
}

void MainMenu::Draw() {
    sf::RenderWindow *window = stateMgr->GetContext()->window->GetRenderWindow();

    //window->draw(text);
    window->draw(bgWindow);
    window->draw(bgMenu);
    RenderButtons(*window);
}

void MainMenu::Activate() {

    if (stateMgr->HasState(StateType::GamePlay)) {
        Button *btn = buttons["Play"];
        btn->UpdateButtonString("Resume");
    }
}

void MainMenu::MouseClick(EventDetails *details) {
    sf::Vector2i mouse{details->mouse};

    for (auto &it : buttons) {
        auto rect = it.second->GetRect();
        if (rect.contains(sf::Vector2f{mouse})) {
            cout << "Pressed " << it.first << endl;
            it.second->SetState(ButtonState::BUTTON_ACTIVE);
            buttonActions[it.first]();
        }
    }
}
