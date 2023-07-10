#include "StateMgr.hpp"

#include "StateIntro.hpp"
#include "MainMenu.hpp"
#include "GamePlay.hpp"
#include "GamePaused.hpp"

StateType StateMgr::currentState = StateType::Invalid;

void StateMgr::CreateState(const StateType &type) {
    auto f = stateFactory.find(type);
    if (f == stateFactory.end()) return;

    State *state = f->second();

    states.emplace_back(type, state);

    currentState = type;

    state->OnCreate();
}

StateMgr::StateMgr(Context *context) : context{context} {
    RegisterState<StateIntro>(StateType::Intro);
    RegisterState<MainMenu>(StateType::MainMenu);
    RegisterState<GamePlay>(StateType::GamePlay);
    RegisterState<GamePaused>(StateType::GamePaused);
}

StateMgr::~StateMgr() {
    for (auto &itr : states) {
        itr.second->OnDestroy();
        delete itr.second;
    }
}

Context *StateMgr::GetContext() const {
    return context;
}

bool StateMgr::HasState(const StateType &type) {
    for (auto &itr : states) {
        if (itr.first == type) {
            return true;
        }
    }
    return false;
}

void StateMgr::SwitchTo(const StateType &type) {
    // First check if the state already exist
    // If exist, then add state at the back
    for (auto itr = states.begin(); itr != states.end(); ++itr) {
        if (itr->first == type) {
            states.back().second->Deactivate();
            State *state = itr->second;
            states.erase(itr);
            states.emplace_back(type, state);
            currentState = type;
            state->Activate();
            return;
        }
    }

    if (!states.empty()) {
        states.back().second->Deactivate();
    }
    // add new state
    CreateState(type);
    states.back().second->Activate();
}

void StateMgr::Update(const sf::Time &time) {
    states.back().second->Update(time);
}

void StateMgr::Draw() {
    if (states.empty())
        return;

    if (states.size() > 1 && states.back().second->IsTransparent()) {
        // Find the first state from the end for which transparent flag is not set
        auto itr = states.end();

        while (itr != states.begin()) {
            --itr;
            if (!itr->second->IsTransparent()) {
                break;
            }
        }

        for (; itr != states.end(); ++itr) {
            itr->second->Draw();
        }

        return;
    }

    states.back().second->Draw();
}
