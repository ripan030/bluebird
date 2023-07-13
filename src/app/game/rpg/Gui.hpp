#ifndef GUI_HPP
#define GUI_HPP

#include <SFML/Graphics.hpp>

enum ButtonState { BUTTON_IDLE, BUTTON_HOVER, BUTTON_ACTIVE };

class Button {
    ButtonState state;

    sf::RectangleShape rect;
    sf::Font *font;
    sf::Text text;
    std::string label;

    sf::Color idleColor;
    sf::Color idleTextColor;
    sf::Color idleOutlineColor;

    sf::Color hoverColor;
    sf::Color hoverTextColor;
    sf::Color hoverOutlineColor;

    sf::Color activeColor;
    sf::Color activeTextColor;
    sf::Color activeOutlineColor;

    void UpdateButton();
public:
    Button() = default;
    Button(float x, float y, float width, float height,
            std::string text, unsigned int textSize, sf::Font *textFont);
    ~Button();
    const sf::FloatRect GetRect() const {
        return rect.getGlobalBounds();
    }
    void SetState(const ButtonState &state) {
        this->state = state;
    }
    ButtonState GetState() const {
        return state;
    }
    void UpdateButtonString(const std::string str);
    void Update(const sf::Vector2i &mousePos);
    void Render(sf::RenderTarget &target);
};
#endif
