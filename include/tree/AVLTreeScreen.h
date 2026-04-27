#pragma once
#include "core/screen.h"
#include "tree/AVLTreeCore.h"
#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>

class AVLTreeScreen : public Screen {
private:
    std::function<void(std::unique_ptr<Screen>)> onChangeScreen;

    AVLTree avlTree;

    // UI State variables
    int inputValue = 0;
    int deleteValue = 0;
    int searchVal = 0;
    int randomCount = 5;
    char filepathBuffer[256] = "data.txt";

    sf::Sprite backgroundSprite;

    // ================= PLAYBACK =================
    bool isPlaying = false;
    int currentStep = 0;
    int totalSteps = 0;

    float playbackSpeed = 1.0f;

    float transitionTime = 0.4f;
    float elapsed = 0.0f;

    void startAnimation();

    // ================= ANIMATION ENGINE =================

    float ease(float t);

    AVLTree::NodeState lerpNode(
        const AVLTree::NodeState& a,
        const AVLTree::NodeState& b,
        float t
    );

    void drawFromFrame(
        sf::RenderWindow& window,
        const std::vector<AVLTree::NodeState>& nodes,
        int idx,
        float x, float y, float offset
    );

public:
    AVLTreeScreen(std::function<void(std::unique_ptr<Screen>)> changeScreenCallback);

    void update(sf::RenderWindow& window, sf::Time deltaTime) override;
    void render(sf::RenderWindow& window) override;
};