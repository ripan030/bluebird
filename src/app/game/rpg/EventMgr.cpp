#include "EventMgr.hpp"
#include "StateMgr.hpp"

EventMgr::EventMgr() {
    LoadBindings();
}

EventMgr::~EventMgr() {
    UnloadBindings();
}

void EventMgr::LoadBindings() {
    Binding *bind;

    bind = new Binding{"PressedSpace"};
    bind->BindEvent(EventType::KeyDown, {sf::Keyboard::Space});
    AddBinding(bind);

    bind = new Binding{"PressedEsc"};
    bind->BindEvent(EventType::KeyDown, {sf::Keyboard::Escape});
    AddBinding(bind);

    bind = new Binding{"PressedP"};
    bind->BindEvent(EventType::KeyDown, {sf::Keyboard::P});
    AddBinding(bind);

    bind = new Binding{"MouseLeft"};
    bind->BindEvent(EventType::MouseButtonDown, {sf::Mouse::Left});
    AddBinding(bind);

    bind = new Binding{"PressedA"};
    bind->BindEvent(EventType::KeyDown, {sf::Keyboard::A});
    AddBinding(bind);

    bind = new Binding{"PressedD"};
    bind->BindEvent(EventType::KeyDown, {sf::Keyboard::D});
    AddBinding(bind);

    bind = new Binding{"ReleasedD"};
    bind->BindEvent(EventType::KeyUp, {sf::Keyboard::D});
    AddBinding(bind);

    bind = new Binding{"PressedW"};
    bind->BindEvent(EventType::KeyDown, {sf::Keyboard::W});
    AddBinding(bind);

    bind = new Binding{"PressedS"};
    bind->BindEvent(EventType::KeyDown, {sf::Keyboard::S});
    AddBinding(bind);
}

void EventMgr::UnloadBindings() {
    for (auto &itr : bindings) {
        delete itr.second;
        itr.second = nullptr;
    }
}

void EventMgr::AddBinding(Binding *bind) {
    bindings[bind->name] = bind;
}

void EventMgr::HandleEvent(sf::Event event) {
    EventType type = (EventType)event.type;

    for (auto &b : bindings) {
        Binding *bind = b.second;

        // find the matching event
        for (auto &e : bind->events) {
            if (type != e.first) {
                continue;
            }

            if (type == EventType::KeyUp || type == EventType::KeyDown) {
                if (event.key.code == e.second.code) {
                    bind->count_events++;
                }

                // Update event details for processing later
                if (bind->details.code == -1) {
                    bind->details.code = e.second.code;
                    bind->details.type = type;
                }
                break;
            } else if (type == EventType::MouseButtonDown ||
                    type == EventType::MouseButtonUp) {
                if (event.key.code == e.second.code) {
                    bind->details.mouse.x = event.mouseButton.x;
                    bind->details.mouse.y = event.mouseButton.y;

                    bind->details.type = type;

                    if (bind->details.code == -1) {
                        bind->details.code = e.second.code;
                    }

                    bind->count_events++;
                    break;
                }
            }
        }
    }
}

void EventMgr::Update() {
    for (auto &b : bindings) {
        Binding *bind = b.second;
        std::string action = b.first;
        if (bind->events.size() == bind->count_events) {
            //std::cout << static_cast<std::underlying_type<StateType>::type>(gCurrentState) << std::endl;
            auto stateCallbacks = callbacks.find(StateMgr::GetCurrent());
            if (stateCallbacks != callbacks.end()) {
                auto it = stateCallbacks->second.find(bind->name);
                if (it != stateCallbacks->second.end()) {
                    it->second(&bind->details);
                }
            }
        }

        bind->count_events = 0;
        bind->details.Clear();
    }
}
