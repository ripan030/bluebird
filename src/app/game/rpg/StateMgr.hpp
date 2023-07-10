#ifndef STATEMGR_HPP
#define STATEMGR_HPP

#include "State.hpp"
#include "EventMgr.hpp"
#include "Window.hpp"

struct Context {
    Window *window;
    EventMgr *eventMgr;
    Context(Window *window, EventMgr *eventMgr) : window{window}, eventMgr{eventMgr} {
    }
};

class StateMgr {
    // Shared context
    Context *context;
    // State Container
    std::vector<std::pair<StateType, State *>> states;
    // State factory
    std::unordered_map<StateType, std::function<State *(void)>> stateFactory;

    void CreateState(const StateType &type);
    template<class T>
    void RegisterState(const StateType &type);
public:
    // Current State
    static StateType currentState;

    StateMgr(Context *context);
    ~StateMgr();
    Context *GetContext() const;
    bool HasState(const StateType &type);
    void SwitchTo(const StateType &type);
    void Update(const sf::Time &time);
    void Draw();
    static StateType GetCurrent() {
        return currentState;
    }
};

template<class T>
void StateMgr::RegisterState(const StateType &type) {
    auto f = [this]() -> State *{ return new T(this); };
    stateFactory[type] = f;
}

#endif
