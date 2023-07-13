#include "Gui.hpp"

#include <iostream>

using namespace std;

Button::Button(float x, float y, float width, float height,
        std::string textStr, unsigned int textSize, sf::Font *textFont) :
    font{font} {

        state = ButtonState::BUTTON_IDLE;

        // Default GUI button colour
        idleColor = sf::Color(70, 70, 70, 0);
        idleTextColor = sf::Color(200, 200, 200, 200);
        idleOutlineColor = sf::Color::Transparent;
        //idleOutlineColor = sf::Color::Red;

        hoverColor = sf::Color(150, 150, 150, 0);
        hoverTextColor = sf::Color(255, 255, 255, 255);
        hoverOutlineColor = sf::Color::Transparent;

        //activeColor = sf::Color(255, 255, 255, 255);
        activeColor = sf::Color::Transparent;
        activeTextColor = sf::Color(20, 20, 20, 0);
        activeOutlineColor = sf::Color::Transparent;

        // Create box
        rect.setPosition(x, y);
        rect.setSize(sf::Vector2f{width, height});
        rect.setOutlineThickness(1.0f);

        // Create text object
        text.setString(textStr);
        text.setFont(*textFont);
        text.setCharacterSize(textSize);

        auto textRect = text.getLocalBounds();

        text.setOrigin(textRect.left + textRect.width / 2.0f,
                textRect.top + textRect.height / 2.0f);

        text.setPosition(rect.getPosition().x + rect.getLocalBounds().width / 2.0f,
                rect.getPosition().y + rect.getLocalBounds().height / 2.0f);
}

Button::~Button() {
}

void Button::Update(const sf::Vector2i &mousePos) {
    /*
     * Update Button Color based on the state
     */
    if (state == ButtonState::BUTTON_ACTIVE) {
        rect.setFillColor(activeColor);
        rect.setOutlineColor(activeOutlineColor);
        text.setFillColor(activeTextColor);
    } else if (state == ButtonState::BUTTON_HOVER) {
        rect.setFillColor(hoverColor);
        rect.setOutlineColor(hoverOutlineColor);
        text.setFillColor(hoverTextColor);
    } else {
        rect.setFillColor(idleColor);
        rect.setOutlineColor(idleOutlineColor);
        text.setFillColor(idleTextColor);
    }
}

void Button::Render(sf::RenderTarget &target) {
    target.draw(rect);
    target.draw(text);
}

void Button::UpdateButtonString(const std::string str) {
    text.setString(str);

    sf::FloatRect textRect = text.getLocalBounds();

    text.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
}
