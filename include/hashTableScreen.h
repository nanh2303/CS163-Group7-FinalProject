#pragma once

#include "screen.h"
#include "hash_table/hash_table.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>
#include <vector>

enum class AnimState {
    IDLE,
    HASHING,
    MOVING_TO_BUCKET,
    TRAVERSING_CHAIN,
    SUCCESS,
    FAIL
};

AnimState currentState = AnimState::IDLE;
sf::Vector2f circlePos;    // Current physical screen position of the tracker
sf::Vector2f targetPos;    // Where the tracker is trying to move to
int targetBucket = -1;     // The index we are currently animating
int currentNodeIndex = -1; // How deep into the chain we are
float animTimer = 0.0f;    // Timer for pauses

enum class OperationType {
    NONE,
    SEARCH,
    INSERT,
    DELETE
};

OperationType currentOp = OperationType::NONE;

class HashTableScreen : public Screen {
private:
    // Screen management
    std::function<void(std::unique_ptr<Screen>)> onChangeScreen;

    // --- Background & Visuals ---
    sf::Sprite backgroundSprite;

    // --- UI State Variables ---
    int inputValue = 0;
    bool isStepByStep = true; // Bound to Execution Mode radio buttons

    // --- Hash Table Data Structure ---
    int tableSize = 5;        // Determines how many buckets to draw
    HashTable hash = new HashTable(tableSize);

    // --- Playback State Variables ---
    bool isPlaying = false;
    int currentStep = 0;
    int totalSteps = 10;
    float playbackSpeed = 1.0f;

public:
    HashTableScreen(std::function<void(std::unique_ptr<Screen>)> changeScreenCallback);

    void update(sf::RenderWindow& window, sf::Time deltaTime) override;
    void render(sf::RenderWindow& window) override;
};