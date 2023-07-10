#include "Window.hpp"

void Window::Create() {
    auto style = isFullScreen ? sf::Style::Fullscreen : sf::Style::Default;
    window.create(sf::VideoMode{size.x, size.y}, title, style);
    window.setFramerateLimit(60);
}

void Window::Destroy() {
    window.close();
}

void Window::Setup(const std::string &title, sf::Vector2u size) {
    this->title = title;
    this->size = size;
    this->isFullScreen = false;
    Create();
}

Window::Window() {
    Setup("Window", {640, 480});
}

Window::Window(const std::string &title, const sf::Vector2u &size)
{
    Setup(title, size);
}

Window::~Window() {
    Destroy();
}

void Window::Close() {
    Destroy();
}

bool Window::IsOpen() {
    return window.isOpen();
}

void Window::ToggleFullScreen() {
    isFullScreen = !isFullScreen;
    Destroy();
    Create();
}

sf::Vector2u Window::GetWindowSize() const {
    return size;
}

sf::RenderWindow *Window::GetRenderWindow() {
    return &window;
}

EventMgr *Window::GetEventMgr() {
    return &eventMgr;
}

void Window::Update() {
    sf::Event event;

    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
       eventMgr.HandleEvent(event);
    }

    eventMgr.Update();
}

void Window::BeginDraw() {
    window.clear();
}

void Window::EndDraw() {
    window.display();
}

void Window::Draw(sf::Drawable &drawable) {
    window.draw(drawable);
}
