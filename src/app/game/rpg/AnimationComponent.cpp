#include "AnimationComponent.hpp"

Animation::Animation(sf::Sprite &sprite, sf::Texture &sheet,
        sf::IntRect startFrame, int nframe,
        float animationTimer) :
    sprite{sprite}, textureShet{sheet},
    startFrame{startFrame}, nframe{nframe}, animationTimer{animationTimer} {
    currFrame = startFrame;
    endFrame = startFrame;
    if (nframe > 1) {
        endFrame.left += (nframe - 1 ) * startFrame.width;
    }
}

Animation::~Animation() {
}

bool Animation::Play(const float &dt) {
    bool done = false;
    timer += dt;
    if (timer > animationTimer) {
        timer = 0;
        if (currFrame != endFrame) {
            currFrame.left += startFrame.width;
        } else {
            currFrame.left = startFrame.left;
            done = true;
        }
    }
    sprite.setTextureRect(currFrame);
    return done;
}

void Animation::Pause() {

}

AnimationComponent::AnimationComponent() {
}

AnimationComponent::~AnimationComponent() {
    for (auto &it : animations) {
        delete it.second;
    }
}

void AnimationComponent::AddAnimation(std::string name, sf::Sprite &sprite, sf::Texture &sheet,
        sf::IntRect startFrame, int nframe, float animationTimer) {
    animations[name] = new Animation{sprite, sheet, startFrame, nframe, animationTimer};
}
