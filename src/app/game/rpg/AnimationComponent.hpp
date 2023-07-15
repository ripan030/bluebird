#ifndef ANIMATION_COMPONENT_HPP
#define ANIMATION_COMPONENT_HPP

#include <SFML/Graphics.hpp>

class Animation {
    sf::Sprite &sprite;
    sf::Texture &textureShet;
    sf::Vector2f position;
    sf::IntRect startFrame;
    sf::IntRect endFrame;
    sf::IntRect currFrame;
    int nframe;
    float timer;
    float animationTimer;
public:
    Animation(sf::Sprite &sprite, sf::Texture &sheet,
            sf::IntRect startFrame, int nframe, float animationTimer);
    ~Animation();
    bool Play(const float &dt);
    void Pause();
};

class AnimationComponent {
public:
    std::unordered_map<std::string, Animation *> animations;
    AnimationComponent();
    ~AnimationComponent();
    void AddAnimation(std::string name, sf::Sprite &sprite, sf::Texture &sheet,
            sf::IntRect startFrame, int nframe, float animationTimer);
};

#endif
