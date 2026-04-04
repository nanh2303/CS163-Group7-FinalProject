#include "HashTableScreen.h"
#include "menuScreen.h"
#include "theme.h"
#include "imgui.h"
#include "assetManager.h"
#include "drawingUtils.h"
#include <string>
#include <cmath>

// --- Constructor ---
HashTableScreen::HashTableScreen(std::function<void(std::unique_ptr<Screen>)> changeScreenCallback)
    : onChangeScreen(std::move(changeScreenCallback)) {
    
    // Set up background
    sf::Texture& bgTexture = AssetManager::getInstance().getTexture("main_bg");
    backgroundSprite.setTexture(bgTexture);
    backgroundSprite.setColor(sf::Color(255, 255, 255, 150)); 

    // --- LAYOUT VARIABLES (Crucial for rendering) ---
    // Push the drawing area to the right of the ImGui panel
    canvasStartX = 350.0f; 
    canvasStartY = 100.0f; 
    verticalSpacing = 60.0f;
    horizontalSpacing = 80.0f;
    
    // Initialize backend Hash Table
    tableSize = 10;
    table = std::make_unique<HashTable>(tableSize);

    // Set up tracking circle
    float trackerRadius = Theme::NodeHeight * 0.6f;
    trackingCircle.setRadius(trackerRadius);
    trackingCircle.setFillColor(sf::Color::Transparent);
    trackingCircle.setOutlineColor(sf::Color::Red); // Or Theme::HighlightColor
    trackingCircle.setOutlineThickness(4.0f);
    trackingCircle.setOrigin(trackerRadius, trackerRadius);
}

// --- Update Loop (Logic & UI) ---
void HashTableScreen::update(sf::RenderWindow& window, sf::Time deltaTime) {
    // Only animate if we are actively playing or stepping through
    if (!isPlaying && !isStepByStep) return;

    if (currentState != AnimState::IDLE) {
        float speed = 200.0f * playbackSpeed; // Base speed in pixels per second
        sf::Vector2f dir = trackerTargetPos - trackerPos;
        float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        if (dist > 1.0f) {
            // Lerp the tracker towards the target
            trackerPos += (dir / dist) * speed * deltaTime.asSeconds();
            trackingCircle.setPosition(trackerPos);
        } else {
            // Reached target! Snap to position and evaluate next step
            trackerPos = trackerTargetPos; 
            
            switch (currentState) {
                case AnimState::HASHING:
                    currentState = AnimState::MOVING_TO_BUCKET;
                    // Move vertically to the correct bucket
                    trackerTargetPos = sf::Vector2f(canvasStartX, canvasStartY + targetBucket * verticalSpacing);
                    if (isStepByStep) isPlaying = false; // Pause for step-by-step
                    break;
                    
                case AnimState::MOVING_TO_BUCKET:
                    currentState = AnimState::TRAVERSING_CHAIN;
                    // Move horizontally to the start of the linked list
                    trackerTargetPos = sf::Vector2f(canvasStartX + targetChainIndex * horizontalSpacing, trackerPos.y);
                    if (isStepByStep) isPlaying = false;
                    break;
                    
                case AnimState::TRAVERSING_CHAIN:
                    // Here you would add logic to check if the current node matches inputKey
                    // For now, we transition to SUCCESS to finish the animation
                    currentState = AnimState::SUCCESS;
                    if (isStepByStep) isPlaying = false;
                    break;

                case AnimState::SUCCESS:
                case AnimState::FAIL:
                    currentState = AnimState::IDLE;
                    isPlaying = false;
                    break;
                    
                default:
                    break;
            }
        }
    }
}

// --- Render Loop (Drawing) ---
void HashTableScreen::render(sf::RenderWindow& window) {
    // ==========================================
    // 1. SFML CANVAS DRAWING
    // ==========================================
    window.draw(backgroundSprite);

    // Setup node styling based on your Theme variables
    sf::RectangleShape nodeBox(sf::Vector2f(Theme::NodeWidth, Theme::NodeHeight)); 
    nodeBox.setFillColor(sf::Color(Theme::NodeDefault.r, Theme::NodeDefault.g, Theme::NodeDefault.b));
    nodeBox.setOutlineThickness(2.0f);
    nodeBox.setOutlineColor(sf::Color::Black);

    // Make sure to load a font in your constructor, e.g., font.loadFromFile("arial.ttf")
    sf::Font font; 
    sf::Text valueText("", font, 16);
    valueText.setFillColor(sf::Color::Black);

    // Draw the Hash Table
    if (table) {
        for (int i = 0; i < tableSize; ++i) {
            float bucketY = canvasStartY + i * verticalSpacing;
            
            // Draw bucket base
            nodeBox.setPosition(canvasStartX, bucketY);
            window.draw(nodeBox);

            // Traverse and draw the separate chaining linked list
            // REQUIRES: table->getBucket(i) to be implemented in HashTable
            Node* current = nullptr; /* table->getBucket(i); */
            int chainDepth = 1;
            
            while (current != nullptr) {
                float nodeX = canvasStartX + chainDepth * horizontalSpacing;
                nodeBox.setPosition(nodeX, bucketY);
                window.draw(nodeBox);

                valueText.setString(std::to_string(current->val));
                valueText.setPosition(nodeX + 10.0f, bucketY + 5.0f);
                window.draw(valueText);

                current = current->next;
                chainDepth++;
            }
        }
    }

    // Draw the Tracker on top of everything
    if (currentState != AnimState::IDLE) {
        window.draw(trackingCircle);
    }

    // ==========================================
    // 2. IMGUI CONTROL PANEL
    // ==========================================
    ImGui::SetNextWindowSizeConstraints(ImVec2(250.0f, window.getSize().y - 20.0f), ImVec2(800.0f, window.getSize().y - 20.0f));
    ImGui::SetNextWindowSize(ImVec2(Theme::ControlPanelWidth, window.getSize().y - 20.0f), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
    ImGui::Begin("Hash Table Controls", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

    Theme::ControlPanelWidth = ImGui::GetWindowWidth();

    if (ImGui::Button("<- Back to Menu")) {
        onChangeScreen(std::make_unique<MenuScreen>(onChangeScreen));
        ImGui::End();
        return;
    }

    ImGui::Separator(); ImGui::Spacing();

    ImGui::Text("Execution Mode:");
    int mode = isStepByStep ? 1 : 0;
    if (ImGui::RadioButton("Run at once", &mode, 0)) isStepByStep = false;
    ImGui::SameLine();
    if (ImGui::RadioButton("Step-by-step", &mode, 1)) isStepByStep = true;
    
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::Text("Data operations:");
    ImGui::InputInt("Target Key", &inputKey);
    ImGui::Spacing();

    ImVec2 opBtnSize(120, 30);
    
    if (ImGui::Button("Insert", opBtnSize)) { 
        // 1. Tell the state machine what we are doing
        currentOp = OperationType::INSERT;
        currentState = AnimState::HASHING;
        
        // 2. Calculate the destination bucket using your new getter
        targetBucket = table->getHash(inputKey); 
        targetChainIndex = 0; // We start at the head of the chain
        
        // 3. Reset the tracking circle to the "start" position above the canvas
        trackerPos = sf::Vector2f(canvasStartX + Theme::NodeWidth / 2.0f, canvasStartY - 50.0f);
        trackingCircle.setPosition(trackerPos);
        
        // 4. Trigger the animation based on the current mode
        isPlaying = !isStepByStep; 
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete", opBtnSize)) { 
        currentOp = OperationType::DELETE;
        currentState = AnimState::HASHING;
        
        targetBucket = table->getHash(inputKey); 
        targetChainIndex = 0; 
        
        trackerPos = sf::Vector2f(canvasStartX + Theme::NodeWidth / 2.0f, canvasStartY - 50.0f);
        trackingCircle.setPosition(trackerPos);
        
        isPlaying = !isStepByStep; 
    }
    if (ImGui::Button("Search", opBtnSize)) {
        currentOp = OperationType::SEARCH;
        currentState = AnimState::HASHING;
        
        // REQUIRES: A public getter for the hash function in your HashTable class
        // targetBucket = table->getHash(inputKey); 
        targetChainIndex = 0; 
        
        trackerPos = sf::Vector2f(canvasStartX + Theme::NodeWidth / 2.0f, canvasStartY - 50.0f);
        trackingCircle.setPosition(trackerPos);
        
        isPlaying = !isStepByStep; 
    }

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    if (ImGui::CollapsingHeader("Appearance Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Node sizes");
        ImGui::SliderFloat("Width", &Theme::NodeWidth, 40.0f, 200.0f);
        ImGui::SliderFloat("Height", &Theme::NodeHeight, 30.0f, 150.0f);
        ImGui::Spacing();

        ImGui::Text("Color");
        float color[3] = { Theme::NodeDefault.r / 255.0f, Theme::NodeDefault.g / 255.0f, Theme::NodeDefault.b / 255.0f };
        if (ImGui::ColorEdit3("Default node", color)) {
            Theme::NodeDefault.r = static_cast<sf::Uint8>(color[0] * 255.0f);
            Theme::NodeDefault.g = static_cast<sf::Uint8>(color[1] * 255.0f);
            Theme::NodeDefault.b = static_cast<sf::Uint8>(color[2] * 255.0f);
        }
    }
    ImGui::End();

    // ==========================================
    // 3. PLAYBACK CONTROL BAR
    // ==========================================
    float bottomBarHeight = 120.0f;
    float canvasWidth = window.getSize().x - Theme::ControlPanelWidth;
    ImGui::SetNextWindowPos(ImVec2(Theme::ControlPanelWidth, window.getSize().y - bottomBarHeight));
    ImGui::SetNextWindowSize(ImVec2(canvasWidth, bottomBarHeight));

    ImGui::Begin("Playback controls", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

    float controlsWidth = 400.0f;
    ImGui::SetCursorPosX((canvasWidth - controlsWidth) * 0.5f);
    ImVec2 btnSize(45, 30);

    if (ImGui::Button(u8"\uf048", btnSize)) {
        isPlaying = false;
        if (currentStep > 0) currentStep--;
    }
    ImGui::SameLine();

    if (isPlaying) {
        if (ImGui::Button(u8"\uf04c", btnSize)) isPlaying = false;
    } else {
        if (ImGui::Button(u8"\uf04b", btnSize)) isPlaying = true;
    }
    ImGui::SameLine();

    if (ImGui::Button(u8"\uf051", btnSize)) {
        isPlaying = false;
        if (currentStep < totalSteps) currentStep++;
    }
    ImGui::SameLine();
    
    ImGui::SetNextItemWidth(120.0f);
    ImGui::SliderFloat("Speed", &playbackSpeed, 0.5f, 3.0f, "%.1fx");
    ImGui::Spacing(); ImGui::Spacing();

    ImGui::SetCursorPosX((canvasWidth - controlsWidth) * 0.5f);
    ImGui::ProgressBar(totalSteps > 0 ? (float)currentStep / (float)totalSteps : 0.0f, ImVec2(controlsWidth, 30));

    ImGui::End();
}