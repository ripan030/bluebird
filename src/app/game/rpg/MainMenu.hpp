#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "State.hpp"
#include "EventMgr.hpp"

class MainMenu : public State {
    sf::Text text;
    sf::Texture introTexture;
    sf::Sprite introSprite;
    sf::Font font;

    sf::Vector2f buttonSize = sf::Vector2f(300.0f, 32.0f);
    sf::RectangleShape rects[2];
    std::vector<std::string> options{"PLAY", "EXIT"};
    sf::Text labels[2];
public:
    MainMenu(StateMgr *stateMgr);
    ~MainMenu();
    void OnCreate() override;
    void OnDestroy() override;
    void Update(const sf::Time &time) override;
    void Draw() override;
    void Activate() override;
    void UserInput(EventDetails *details);
    void MouseClick(EventDetails *details);
};
#endif
