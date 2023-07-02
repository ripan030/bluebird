#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cstdlib>

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

int main()
{
    sf::RenderWindow window{sf::VideoMode{640, 480}, "Space WAR", sf::Style::Default};

    window.setFramerateLimit(60);

    sf::Texture playerTexture, enemyTexture, bulletTexture;

    playerTexture.loadFromFile("Textures/spaceship.png");
    enemyTexture.loadFromFile("Textures/enemy_spaceship.png");
    bulletTexture.loadFromFile("Textures/bullet.png");

    Player player{&window, &playerTexture};
    Enemy enemy{&window, &enemyTexture};

    int shootTimer = 10;
    int enemySpawnTimer = 80;

    std::vector<Enemy> enemies;

    auto render = [&window, &player, &enemies]() {
        window.clear(sf::Color::Transparent);
        player.draw();
        for (auto &enemy : enemies) {
           enemy.draw();
        }

        window.display();
    };

    srand(time(NULL));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            player.move_left();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            player.move_right();

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && shootTimer >= 10) {
            shootTimer = 0;
            Bullet bullet{&window, &bulletTexture};
            player.shoot(bullet);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && shootTimer >= 10) {
            shootTimer = 0;
            Bullet bullet{&window, &bulletTexture};
            player.shoot(bullet);
        }

        if (shootTimer < 10)
            shootTimer++;

        if (enemySpawnTimer > 0)
            enemySpawnTimer--;

        if (!enemySpawnTimer) {
            enemySpawnTimer = 40;
            enemies.emplace_back(&window, &enemyTexture);
        }

        player.update();

        for (auto it = enemies.begin(); it != enemies.end(); ) {
           if (!(*it).getDestroy()) {
               player.update(*it);
           }

           (*it).update();

           if ((*it).getDestroy()) {
               enemies.erase(it);
           } else {
               it++;
           }
        }

        render();
    }

    return 0;
}
