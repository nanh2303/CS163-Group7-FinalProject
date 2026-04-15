#pragma once 
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>
#include "screen.h"
#include "hashTableCore.h"


class HashTableScreen : public Screen {
private:
    HashTable hashTable;

    std::function<void(std::unique_ptr<Screen>)> onChangeScreen;
    sf::Sprite backgroundSprite;

    bool isTableInitialized = false;

    bool isPlaying = false;
    int currentStep = 0;
    int totalSteps = 0;
    float timer = 0.0f;
    float playbackSpeed = 1.0f;

    int tableCapacity = 0;
    int inputValue = 0;
    int deleteValue = 0;
    int searchValue = 0;

    int updateOldValue = 0;
    int updateNewValue = 0;

    char filePath[256] = "data.txt";

    void startAnimation();

public:
    explicit HashTableScreen(std::function<void(std::unique_ptr<Screen>)> changeScreenCallback);

    void update(sf::RenderWindow& window, sf::Time deltaTime) override;
    void render(sf::RenderWindow& window) override;
};