#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "State.hpp"
#include "EventMgr.hpp"
#include "Gui.hpp"

class MainMenu : public State {
    sf::Text text;
    sf::Texture introTexture;
    sf::Sprite introSprite;
    sf::Font font;

    sf::RectangleShape bgWindow;
    sf::RectangleShape bgMenu;

    std::unordered_map<std::string, Button *> buttons;
    std::unordered_map<std::string, std::function<void()>> buttonActions;

    void InitGui();
    void ResetGui();
    void UpdateButtons();
    void RenderButtons(sf::RenderTarget &target);
public:
    MainMenu(StateMgr *stateMgr);
    ~MainMenu();
    void OnCreate() override;
    void OnDestroy() override;
    void Update(const sf::Time &time) override;
    void Draw() override;
    void Activate() override;
    void MouseClick(EventDetails *details);
};
#endif
