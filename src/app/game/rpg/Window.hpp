#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <SFML/Graphics.hpp>

#include "EventMgr.hpp"

class Window {
    sf::RenderWindow window;
    sf::Vector2u size;
    std::string title;
    bool isFullScreen;
    EventMgr eventMgr;

    void Create();
    void Destroy();
    void Setup(const std::string &title, sf::Vector2u size);
public:
    Window();
    Window(const std::string &title, const sf::Vector2u &size);
    ~Window();
    void Close();
    bool IsOpen();
    void ToggleFullScreen();
    sf::Vector2u GetWindowSize() const;
    sf::RenderWindow *GetRenderWindow();
    EventMgr *GetEventMgr();
    void Update();
    void Draw(sf::Drawable &drawable);
    void BeginDraw();
    void EndDraw();
};
#endif
