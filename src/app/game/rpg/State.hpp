#ifndef STATE_HPP
#define STATE_HPP

#include <SFML/Graphics.hpp>

enum class StateType {
    Invalid = 0,
    Intro,
    MainMenu,
    GamePlay,
    GamePaused,
    GameOver,
    Credit
};

class StateMgr;

class State {
    friend class StateMgr;
protected:
    StateMgr *stateMgr;
    bool transparent;
public:
    State(StateMgr *stateMgr) : stateMgr{stateMgr}, transparent{false} {}
    virtual ~State() {}
    virtual void OnCreate() = 0;
    virtual void OnDestroy() = 0;
    virtual void Activate() {}
    virtual void Deactivate() {}
    virtual void Update(const sf::Time &time) = 0;
    virtual void Draw() = 0;
    void SetTransparent(const bool &trans) {
        transparent = trans;
    }
    bool IsTransparent() {
        return transparent;
    }
};
#endif
