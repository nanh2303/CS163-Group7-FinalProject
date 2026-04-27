#pragma once
#include "core/screen.h"
#include <functional>
#include <memory>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

class MenuScreen : public Screen {
private:
    std::function<void(std::unique_ptr<Screen>)> onChangeScreen;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

public:
    MenuScreen(std::function<void(std::unique_ptr<Screen>)> changeScreenCallback);

    void update(sf::RenderWindow& window, sf::Time deltaTime) override;
    void render(sf::RenderWindow& window) override;
};