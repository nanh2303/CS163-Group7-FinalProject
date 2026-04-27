#pragma once
#include "screen.h"
#include "linkedListCore.h"
#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>

class LinkedListScreen : public Screen {
private:
    std::function<void(std::unique_ptr<Screen>)> onChangeScreen;

    LinkedList linkedList;
    // UI State variables
    int inputValue = 0;
    int deleteValue = 0; 
    int searchVal = 0;   
    int updateOldVal = 0;
    int updateNewVal = 0;
    int randomCount = 5;
    char filepathBuffer[256] = "data.txt";

	sf::Sprite backgroundSprite;

    // Playback state variables
    bool isPlaying = false;
    int currentStep = 0;
    int totalSteps = 0;
    float playbackSpeed = 1.0f;
	float timer = 0.0f; //To handle playback speed

    void startAnimation();

public:
    LinkedListScreen(std::function<void(std::unique_ptr<Screen>)> changeScreenCallback);

    void update(sf::RenderWindow& window, sf::Time deltaTime) override;
    void render(sf::RenderWindow& window) override;
};