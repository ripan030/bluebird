#include <SFML/Graphics.hpp>

#include <State.hpp>
#include <EventMgr.hpp>
#include <Window.hpp>
#include <StateMgr.hpp>

class Game {
    Context context;
    Window window;
    StateMgr stateMgr;
    sf::Clock clock;
public:
    Game();
    ~Game();
    void Update(const sf::Time &time);
    void Render();

    sf::Time RestartClock() {
        return clock.restart();
    }

    Window &GetWindow() {
        return window;
    }
};

Game::Game() : window{"Gun Man", sf::Vector2u{640, 480}},
    context{&window, window.GetEventMgr()},
    stateMgr{&context} {
    stateMgr.SwitchTo(StateType::Intro);
}

Game::~Game() {

}

void Game::Update(const sf::Time &time) {
    window.Update(); // poll for event and deliver event to the respective state handler
    stateMgr.Update(time); // update states
}

void Game::Render() {
    window.BeginDraw();
    stateMgr.Draw();
    window.EndDraw();
}

int main() {
    Game game;

    while (game.GetWindow().IsOpen()) {
        game.Update(game.RestartClock());
        game.Render();
    }

    return 0;
}
