#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include <iostream>
using namespace std;

class Enemy {
public:
    sf::Sprite shape;
    sf::Texture *texture;
    sf::RenderWindow *window;
    bool destroy = false;

    Enemy(sf::RenderWindow *window, sf::Texture *texture) : window{window}, texture{texture} {
       shape.setTexture(*texture);
       shape.setScale(sf::Vector2f{0.08f, 0.08f});

       shape.setPosition(rand() % (window->getSize().x), 0);
    }

    void update() {
        shape.move(0.f, 0.2f);
    }

    void setDestroy(bool val) {
        destroy = val;
    }

    bool getDestroy() const {
        return destroy;
    }

    void draw() {
        window->draw(shape);
    }

    ~Enemy() {}
};

class Bullet {
public:
    sf::Sprite shape;
    sf::Texture *texture;
    sf::RenderWindow *window;
    sf::Vector2f current_pos;

    Bullet(sf::RenderWindow *window, sf::Texture *texture) : window{window}, texture{texture} {
       shape.setTexture(*texture);
       shape.setScale(sf::Vector2f{0.05f, 0.02f});
    }

    void setPosition(sf::Vector2f pos) {
        current_pos = pos;
        shape.setPosition(current_pos.x, current_pos.y);
    }

    void update() {
        current_pos.y -= 2;
        setPosition(current_pos);
    }

    bool isOutOfWindow() {
        if (current_pos.y < 0)
            return true;
        else
            return false;
    }

    bool isIntersects(sf::FloatRect rect) {
        return shape.getGlobalBounds().intersects(rect);
    }

    void draw() {
        window->draw(shape);
    }

    ~Bullet() {}
};

class Player {
public:
    sf::Sprite shape;
    sf::Texture *texture;
    sf::RenderWindow *window;

    std::vector<Bullet> bullets;

    Player(sf::RenderWindow *window, sf::Texture *texture) : window{window}, texture{texture} {
       shape.setTexture(*texture);
       shape.setScale(sf::Vector2f{0.1f, 0.1f});
       shape.setPosition(window->getSize().x / 2 - shape.getGlobalBounds().width / 2,  window->getSize().y - shape.getGlobalBounds().height);
    }

    void move_left() {
        shape.move(-2.f, 0.f);
    }

    void move_right() {
        shape.move(2.f, 0.f);
    }

    void update() {
        for (auto it = bullets.begin(); it != bullets.end();) {
            (*it).update();
            if ((*it).isOutOfWindow()) {
                bullets.erase(it);
            } else {
                ++it;
            }
        }
    }

    void update(Enemy &enemy) {
        for (auto it = bullets.begin(); it != bullets.end();) {
            if ((*it).isIntersects(enemy.shape.getGlobalBounds())) {
                bullets.erase(it);
                enemy.setDestroy(true);
            } else {
                ++it;
            }
        }
    }

    void shoot(Bullet bullet) {
        bullet.setPosition(sf::Vector2f{shape.getPosition().x + shape.getGlobalBounds().width / 4,
                shape.getPosition().y});
        bullets.push_back(bullet);
    }

    void draw() {
        window->draw(shape);
        for (auto &bullet : bullets) {
            bullet.draw();
        }
    }

    ~Player() {}
};

struct State {
    State() {}
    virtual ~State() {}
    virtual void init() = 0;
    virtual void processUserInput() =0;
    virtual void update() = 0;
    virtual void draw() =0;
    virtual void exit() {};
};

class StateMgr {
    std::unique_ptr<State> prev;
    std::unique_ptr<State> curr;
public:
    StateMgr() {}
    ~StateMgr() {}
    void add(std::unique_ptr<State> state) {
        //curr->exit();
        //prev = std::move(curr);
        curr = std::move(state);
        curr->init();
    }

    const std::unique_ptr<State> &getCurrent() const {
        return curr;
    }
};

struct Context {
    std::unique_ptr<sf::RenderWindow> window;
    std::unique_ptr<StateMgr> statemgr;
    Context() : window{std::make_unique<sf::RenderWindow>()},
        statemgr{std::make_unique<StateMgr>()} {}
};

// Main menu
class MainMenu : public State {
    sf::Font font;
    sf::Text title, playButton, exitButton;
    std::shared_ptr<Context> context;
    bool playButtonPressed;
    bool exitButtonPressed;
public:
    MainMenu(std::shared_ptr<Context> context);
    ~MainMenu();
    void init() override;
    void processUserInput() override;
    void update() override;
    void draw() override;
};

// GamePlay
class GamePlay : public State {
    bool escButtonPressed;
    bool pauseButtonPressed;
    bool leftButtonPressed;
    bool rightButtonPressed;
    bool shootButtonPressed;
    std::shared_ptr<Context> context;

    sf::Texture playerTexture, enemyTexture, bulletTexture;
    Player *player;
    std::vector<Enemy> enemies;
public:
    GamePlay(std::shared_ptr<Context> context);
    ~GamePlay();
    void init() override;
    void processUserInput() override;
    void update() override;
    void draw() override;
};

MainMenu::MainMenu(std::shared_ptr<Context> context) : context{context} {
    font.loadFromFile("fonts/arial.ttf");

    title.setString("SPACE WAR");
    title.setFont(font);
    title.setFillColor(sf::Color::Yellow);
    title.setCharacterSize(24);
    title.setOrigin(title.getLocalBounds().width / 2, title.getLocalBounds().height / 2);
    title.setPosition(context->window->getSize().x / 2, 20.f);

    playButton.setString("PLAY (Press UP)");
    playButton.setFont(font);
    playButton.setFillColor(sf::Color::Yellow);
    playButton.setCharacterSize(24);
    playButton.setOrigin(playButton.getLocalBounds().width / 2, playButton.getLocalBounds().height / 2);
    playButton.setPosition(context->window->getSize().x / 2, context->window->getSize().y / 2  -20.f);

    exitButton.setString("EXIT (Press DOWN)");
    exitButton.setFont(font);
    exitButton.setFillColor(sf::Color::Yellow);
    exitButton.setCharacterSize(24);
    exitButton.setOrigin(exitButton.getLocalBounds().width / 2, exitButton.getLocalBounds().height / 2);
    exitButton.setPosition(context->window->getSize().x / 2, context->window->getSize().y / 2 + 20.f);
}

MainMenu::~MainMenu() {
}

void MainMenu::init() {
    cout << "Init Main Menu\n";
}

void MainMenu::processUserInput() {
    playButtonPressed = false;
    exitButtonPressed = false;
    sf::Event event;

    while (context->window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            context->window->close();
        } else if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
            case sf::Keyboard::Up:
                playButtonPressed = true;
                break;
            case sf::Keyboard::Down:
                exitButtonPressed = true;
                break;
            default:
                ;
            }
        }
    }
}

void MainMenu::update() {
   if (playButtonPressed) {
       context->statemgr->add(std::make_unique<GamePlay>(context));
   } else if (exitButtonPressed) {
       context->window->close();
   }
}

void MainMenu::draw() {
    context->window->clear();
    context->window->draw(title);
    context->window->draw(playButton);
    context->window->draw(exitButton);
    context->window->display();
}

GamePlay::GamePlay(std::shared_ptr<Context> context) : context{context} {
    cout << "Init Game Play\n";

    playerTexture.loadFromFile("Textures/spaceship.png");
    enemyTexture.loadFromFile("Textures/enemy_spaceship.png");
    bulletTexture.loadFromFile("Textures/bullet.png");

    player = new Player{context->window.get(), &playerTexture};
}

GamePlay::~GamePlay() {
    delete player;
}

void GamePlay::init() {
}

void GamePlay::processUserInput() {
    escButtonPressed = false;
    pauseButtonPressed = false;
    leftButtonPressed = false;
    rightButtonPressed = false;
    shootButtonPressed = false;
    sf::Event event;

    while (context->window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            context->window->close();
        } else if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
            case sf::Keyboard::A:
                leftButtonPressed = true;
                break;
            case sf::Keyboard::D:
                rightButtonPressed = true;
                break;
            case sf::Keyboard::P:
                pauseButtonPressed = true;
                break;
            case sf::Keyboard::Escape:
                escButtonPressed = true;
                break;
            default:
                break;
            }
        } else if (event.type == sf::Event::MouseButtonPressed) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                shootButtonPressed = true;
        }
    }
}

void GamePlay::update() {
    static int shootTimer = 0;
    static int enemySpawnTimer = 0;

    if (shootTimer > 0)
        shootTimer--;

    if (enemySpawnTimer > 0)
        enemySpawnTimer--;

    if (escButtonPressed)
        context->window->close();
    else if (leftButtonPressed)
        player->move_left();
    else if (rightButtonPressed)
        player->move_right();
    else if (shootButtonPressed && shootTimer == 0) {
        shootTimer = 0;
        Bullet bullet{context->window.get(), &bulletTexture};
        player->shoot(bullet);
    }

    if (!enemySpawnTimer) {
        enemySpawnTimer = 40;
        enemies.emplace_back(context->window.get(), &enemyTexture);
    }

    player->update();

    for (auto it = enemies.begin(); it != enemies.end(); ) {
       if (!(*it).getDestroy()) {
           player->update(*it);
       }

       (*it).update();

       if ((*it).getDestroy()) {
           enemies.erase(it);
       } else {
           it++;
       }
    }
}

void GamePlay::draw() {
    context->window->clear();
    player->draw();
    for (auto &enemy : enemies) {
       enemy.draw();
    }
    context->window->display();
}

class Game {
    std::shared_ptr<Context> context;
public:
    Game() : context{std::make_shared<Context>()} {
        context->window->create(sf::VideoMode{640, 480}, "Space WAR");
        context->window->setFramerateLimit(60);

        context->statemgr->add(std::make_unique<MainMenu>(context));
    }
    ~Game() {}
    void run() {
        while (context->window->isOpen()) {
            context->statemgr->getCurrent()->processUserInput();
            context->statemgr->getCurrent()->update();
            context->statemgr->getCurrent()->draw();
        }
    }
};

int main() {
    Game spacewar;

    spacewar.run();

    return 0;
}
