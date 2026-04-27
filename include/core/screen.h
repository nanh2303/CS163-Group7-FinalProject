#pragma once
#include <SFML/Graphics.hpp>

class Screen {
public:
    virtual ~Screen() = default;

    virtual void update(sf::RenderWindow& window, sf::Time deltaTime) = 0;

    virtual void render(sf::RenderWindow& window) = 0;
};