#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "screen.h"

class Application {
private:
    sf::RenderWindow window;
    std::unique_ptr<Screen> currentScreen;
    sf::Clock deltaClock;

public:
    Application();
    ~Application();

    void run();
    void changeScreen(std::unique_ptr<Screen> newScreen);
};