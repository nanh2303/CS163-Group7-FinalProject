#pragma once
#include "screen.h"
#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>

class LinkedListScreen : public Screen {
private:
    std::function<void(std::unique_ptr<Screen>)> onChangeScreen;

    // UI State variables
    int inputValue = 0;
	sf::Sprite backgroundSprite;

    // Playback state variables
    bool isPlaying = false;
    int currentStep = 0;
    int totalSteps = 10; // Dummy value for testing
    float playbackSpeed = 1.0f;

public:
    LinkedListScreen(std::function<void(std::unique_ptr<Screen>)> changeScreenCallback);

    void update(sf::RenderWindow& window, sf::Time deltaTime) override;
    void render(sf::RenderWindow& window) override;
};