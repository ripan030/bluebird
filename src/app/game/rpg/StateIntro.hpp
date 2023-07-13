#ifndef STATEINTRO_HPP
#define STATEINTRO_HPP

#include "State.hpp"
#include "EventMgr.hpp"

class StateIntro : public State {
    float timePassed;
    float rotationAngle;
    sf::Text text;
    sf::Texture introTexture;
    sf::Sprite introSprite;
    sf::Font font;
    const float textDisplay = 10.0f;
public:
    StateIntro(StateMgr *stateMgr);
    ~StateIntro();
    void OnCreate() override;
    void OnDestroy() override;
    void Update(const sf::Time &time) override;
    void Draw() override;
    void Continue(EventDetails *details);
};

#endif
