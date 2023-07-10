#ifndef GAMEPLAY_HPP
#define GAMEPLAY_HPP

#include "State.hpp"
#include "EventMgr.hpp"

class GamePlay : public State {
    sf::Texture playTexture;
    sf::Sprite playSprite;
    float rotationAngle;
public:
    GamePlay(StateMgr *stateMgr);
    ~GamePlay();
    void OnCreate() override;
    void OnDestroy() override;
    void Update(const sf::Time &time) override;
    void Draw() override;
    void MainMenu(EventDetails *details);
    void GamePause(EventDetails *details);
};

#endif
