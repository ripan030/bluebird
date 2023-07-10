#ifndef GAMEPAUSED_HPP
#define GAMEPAUSED_HPP

#include "State.hpp"
#include "EventMgr.hpp"

class GamePaused : public State {
    sf::Font font;
    sf::Text text;
    sf::RectangleShape rect;
public:
    GamePaused(StateMgr *stateMgr);
    ~GamePaused();
    void Unpause(EventDetails *details);
    void OnCreate();
    void OnDestroy();
    void Update(const sf::Time &time);
    void Draw();
};

#endif
