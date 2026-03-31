#include "LinkedListScreen.h"
#include "menuScreen.h"
#include "theme.h"
#include "imgui.h"
#include "assetManager.h"
#include "drawingUtils.h"

LinkedListScreen::LinkedListScreen(std::function<void(std::unique_ptr<Screen>)> changeScreenCallback)
    : onChangeScreen(std::move(changeScreenCallback)) {
    // Set up background
    // Fetch the exact same texture the Menu uses
    sf::Texture& bgTexture = AssetManager::getInstance().getTexture("main_bg");
    backgroundSprite.setTexture(bgTexture);

    // Apply the "Fade" effect
    // In SFML, the color (R, G, B, A) acts as a multiplier. 
    // Setting Alpha (the 4th number) to 150 (out of 255) makes it roughly 60% opaque
    // This gives it that "faded" look without modifying the image file.
    backgroundSprite.setColor(sf::Color(255, 255, 255, 150));
}

void LinkedListScreen::update(sf::RenderWindow& window, sf::Time deltaTime) {
    (void)deltaTime;

    //CONTROL PANEL SETUP
    // Lock the height to the window, but let the user drag the width between 250px and 800px
    ImGui::SetNextWindowSizeConstraints(
        ImVec2(250.0f, window.getSize().y - 20.0f),
        ImVec2(800.0f, window.getSize().y - 20.0f)
    );

    // Set the initial starting size (only happens once when the screen loads)
    ImGui::SetNextWindowSize(ImVec2(Theme::ControlPanelWidth, window.getSize().y - 20.0f), ImGuiCond_Once);

    // Force the panel to stay snapped to the top-left corner
    ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);

    // We added NoMove so they can't accidentally drag the window off the screen
    ImGui::Begin("Linked list controls", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

    // Read the current width so the SFML canvas knows where to draw the nodes
    Theme::ControlPanelWidth = ImGui::GetWindowWidth();

    //BACK BUTTON
    if (ImGui::Button("<- Back to Menu")) {
        onChangeScreen(std::make_unique<MenuScreen>(onChangeScreen));
        ImGui::End();
        return;
    }

    ImGui::Separator();
    ImGui::Spacing();

	//DATA INPUT
    ImGui::Text("Data operations:");
    ImGui::InputInt("Value", &inputValue);
    ImGui::Spacing();

    if (ImGui::Button("Add to Head", ImVec2(150, 30))) {}
    ImGui::SameLine();
    if (ImGui::Button("Add to Tail", ImVec2(150, 30))) {}

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    //THEME CUSTOMIZATION
    if (ImGui::CollapsingHeader("Appearance Settings", ImGuiTreeNodeFlags_DefaultOpen)) {

        // Size sliders
        ImGui::Text("Node sizes");
        ImGui::SliderFloat("Width", &Theme::NodeWidth, 40.0f, 200.0f);
        ImGui::SliderFloat("Height", &Theme::NodeHeight, 30.0f, 150.0f);

        ImGui::Spacing();

        // Color pickers
        ImGui::Text("Color");
        float color[3] = { Theme::NodeDefault.r / 255.0f, Theme::NodeDefault.g / 255.0f, Theme::NodeDefault.b / 255.0f };
        if (ImGui::ColorEdit3("Default node", color)) {
            Theme::NodeDefault.r = static_cast<sf::Uint8>(color[0] * 255.0f);
            Theme::NodeDefault.g = static_cast<sf::Uint8>(color[1] * 255.0f);
            Theme::NodeDefault.b = static_cast<sf::Uint8>(color[2] * 255.0f);
        }
    }
    ImGui::End();

    //PLAYBACK CONTROL BAR
    float bottomBarHeight = 120.0f;
    float canvasWidth = window.getSize().x - Theme::ControlPanelWidth;
    // Position it at the bottom, starting after the left panel
    ImGui::SetNextWindowPos(ImVec2(Theme::ControlPanelWidth, window.getSize().y - bottomBarHeight));
    ImGui::SetNextWindowSize(ImVec2(canvasWidth, bottomBarHeight));

    // No background, just floating controls!
    ImGui::Begin("Playback controls", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

    // Center the controls horizontally
    float controlsWidth = 400.0f;
    ImGui::SetCursorPosX((canvasWidth - controlsWidth) * 0.5f);

    ImVec2 btnSize(45, 30);

    // Step back button
    if (ImGui::Button(u8"\uf048", btnSize)) {
        isPlaying = false;
        if (currentStep > 0) currentStep--;
    }

    ImGui::SameLine();

    // Play / pause button
    if (isPlaying) {
        if (ImGui::Button(u8"\uf04c", btnSize)) isPlaying = false;
    }
    else {
        if (ImGui::Button(u8"\uf04b", btnSize)) isPlaying = true;
    }

    ImGui::SameLine();

    // Step forward button
    if (ImGui::Button(u8"\uf051", btnSize)) {
        isPlaying = false;
        if (currentStep < totalSteps) currentStep++;
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(120.0f);
    ImGui::SliderFloat("Speed", &playbackSpeed, 0.5f, 3.0f, "%.1fx");
	ImGui::Spacing(); ImGui::Spacing();

    // Progress bar showing the current frame
    ImGui::SetCursorPosX((canvasWidth - controlsWidth) * 0.5f);
    ImGui::ProgressBar(totalSteps > 0 ? (float)currentStep / (float)totalSteps : 0.0f, ImVec2(controlsWidth, 30));

    ImGui::End();
}

void LinkedListScreen::render(sf::RenderWindow& window) {
	window.draw(backgroundSprite);

    float centerX = (window.getSize().x + Theme::ControlPanelWidth) / 2.0f;
    float centerY = window.getSize().y / 2.0f;

    sf::Vector2f node1Pos(centerX - 100.0f, centerY);
    DrawingUtils::drawRectNode(window, node1Pos, "42", Theme::NodeDefault);

    sf::Vector2f node2Pos(centerX + 100.0f, centerY);
    DrawingUtils::drawRectNode(window, node2Pos, "88", Theme::NodeDefault);

    sf::Vector2f arrowStart(node1Pos.x + Theme::NodeWidth / 2.0f, node1Pos.y);
    sf::Vector2f arrowEnd(node2Pos.x - Theme::NodeWidth / 2.0f, node2Pos.y);
    DrawingUtils::drawArrow(window, arrowStart, arrowEnd, Theme::LinkDefault);
}