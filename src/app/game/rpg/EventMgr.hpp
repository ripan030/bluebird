#ifndef EVENTMGR_HPP
#define EVENTMGR_HPP

#include <SFML/Graphics.hpp>

#include "State.hpp"

/*
 * Event types
 */
enum class EventType {
    KeyDown = sf::Event::KeyPressed,
    KeyUp = sf::Event::KeyReleased,
    MouseButtonDown = sf::Event::MouseButtonPressed,
    MouseButtonUp = sf::Event::MouseButtonReleased,
};

/*
 * Event code
 */
struct EventInfo {
    EventInfo() : code{0} {}
    EventInfo(int code) : code{code} {}
    int code;
};

/*
 * Each event is defined by a event type and code
 */
using Events = std::vector<std::pair<EventType, EventInfo>>;

/*
 * Key is binded to a action (e.g., PressedKeyX)
 * and diffrent callbacks can be registered for different states
 * Binding will store events in EventDetails struct
 */
struct EventDetails {
    EventDetails(const std::string &name) : name{name}, code{-1} {}
    void Clear() {
        code = -1;
        mouse = {0, 0};
    }
    std::string name; // action name
    int code; //keyboard/mouse key code
    sf::Vector2i mouse; // mouse position
};

struct Binding {
    std::string name; // action such as Jumping
    Events events; // Multiple keys (Ctrl + J for jumping) can be bind to same action
    EventDetails details;
    int count_events;

    Binding(const std::string &name) : name{name}, details{name} {}
    void BindEvent(EventType type, EventInfo info = {}) {
       events.emplace_back(type, info);
    }
};

using Bindings = std::unordered_map<std::string /* action name */, Binding *>;

/* Callback */
using CallbackContainer = std::unordered_map<std::string /* action name */, std::function<void(EventDetails *)> /* callback function */>;
using Callbacks = std::unordered_map<StateType, CallbackContainer>;

/*
 * EventMgr class
 */
class EventMgr {
    // Bindings
    Bindings bindings;
    // Callbacks
    Callbacks callbacks;
public:
    EventMgr();
    ~EventMgr();
    void LoadBindings();
    void UnloadBindings();
    void HandleEvent(sf::Event event);
    void Update();
    void AddBinding(Binding *bind);
    template<class T>
    void AddCallback(StateType state, std::string name, void (T::*func)(EventDetails *), T *ins);
};

template<class T>
void EventMgr::AddCallback(StateType state, std::string name, void (T::*func)(EventDetails *), T *ins) {
    auto t = std::bind(func, ins, std::placeholders::_1);
    auto it = callbacks.emplace(state, CallbackContainer{}).first; // get the iter
    it->second.emplace(name, t);
}

#endif
