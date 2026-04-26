#include "LinkedListScreen.h"
#include "menuScreen.h"
#include "theme.h"
#include "imgui.h"
#include "assetManager.h"
#include "drawingUtils.h"
#include "fileDialog.h"

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

void LinkedListScreen::startAnimation() {
    if (linkedList.hasFrames()) {
        totalSteps = linkedList.getFrames().size() - 1;
        currentStep = 0;
        isPlaying = true;
        timer = 0.0f;
    }
}

void LinkedListScreen::update(sf::RenderWindow& window, sf::Time deltaTime) {
    (void)deltaTime;
    if (isPlaying && totalSteps > 0) {
        timer += deltaTime.asSeconds() * playbackSpeed;
        if (timer >= 1.0f) { //1s / frame (scaled by speed)
            timer = 0.0f;
            if (currentStep < totalSteps) currentStep++;
            else isPlaying = false;
        }
    }

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

	//DATA OPERATIONS
    ImGui::Text("Data operations:");
    // Initialization
    if (ImGui::CollapsingHeader("Initialize", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("Empty list", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            linkedList.initEmpty();
            currentStep = 0;
            totalSteps = 0;
            isPlaying = false;
        }

        ImGui::Spacing();
        ImGui::InputInt("Random count", &randomCount);
        if (ImGui::Button("Generate random", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            linkedList.initRandom(randomCount);
            startAnimation();
        }

        ImGui::Spacing();

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 90.0f);
        ImGui::InputText("##filepath", filepathBuffer, IM_ARRAYSIZE(filepathBuffer));

        ImGui::SameLine();

        if (ImGui::Button("Browse", ImVec2(85.0f, 0))) {
        #ifdef _WIN32
            std::string selectedFile = openFileDialog();
            if (!selectedFile.empty()) {
                strncpy(filepathBuffer, selectedFile.c_str(), sizeof(filepathBuffer) - 1);
                filepathBuffer[sizeof(filepathBuffer) - 1] = '\0';
            }
        #endif
        }

        if (ImGui::Button("Load from file", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            linkedList.initFromFile(filepathBuffer);
            startAnimation();
        }
    }

    // Insertion
    if (ImGui::CollapsingHeader("Add", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputInt("Add value", &inputValue);
        if (ImGui::Button("Add to Head", ImVec2(140, 0))) {
            linkedList.insertHead(inputValue);
            startAnimation();
        }
        ImGui::SameLine();
        if (ImGui::Button("Add to Tail", ImVec2(140, 0))) {
            linkedList.insertTail(inputValue);
            startAnimation();
        }
    }

    // Deletion
    if (ImGui::CollapsingHeader("Delete", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputInt("Delete value", &deleteValue);
        if (ImGui::Button("Delete first match", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            linkedList.deleteValue(deleteValue);
            startAnimation();
        }
    }

    // Update
    if (ImGui::CollapsingHeader("Update", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputInt("Old value", &updateOldVal);
        ImGui::InputInt("New value", &updateNewVal);
        if (ImGui::Button("Update node", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            linkedList.updateValue(updateOldVal, updateNewVal);
            startAnimation();
        }
    }

    // Search
    if (ImGui::CollapsingHeader("Search", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputInt("Search value", &searchVal);
        if (ImGui::Button("Find node", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            linkedList.searchValue(searchVal);
            startAnimation();
        }
    }

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

    // PLAYBACK CONTROL BAR
    float bottomBarHeight = 120.0f;

    float middleCanvasWidth = window.getSize().x - Theme::ControlPanelWidth - Theme::CodePanelWidth;

    ImGui::SetNextWindowPos(ImVec2(Theme::ControlPanelWidth, window.getSize().y - bottomBarHeight));
    ImGui::SetNextWindowSize(ImVec2(middleCanvasWidth, bottomBarHeight));

    ImGui::Begin("Playback controls", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

    ImVec2 btnSize(45, 30);

    float styleSpacing = ImGui::GetStyle().ItemSpacing.x;
    float totalTopRowWidth = (btnSize.x * 4.0f) + 120.0f + 50.0f + (styleSpacing * 4.0f);

    ImGui::SetCursorPosX((middleCanvasWidth - totalTopRowWidth) * 0.5f);

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

    // Skip to end button
    if (ImGui::Button(u8"\uf050", btnSize)) {
        isPlaying = false;
        currentStep = totalSteps;
    }

    // Speed slider
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120.0f);
    ImGui::SliderFloat("Speed", &playbackSpeed, 0.5f, 3.0f, "%.1fx");

    ImGui::Spacing();
    ImGui::Spacing();

    float progressBarWidth = 400.0f;
    ImGui::SetCursorPosX((middleCanvasWidth - progressBarWidth) * 0.5f);

    float progress = (totalSteps > 0) ? (float)currentStep / (float)totalSteps : 0.0f;

    char progressText[32];
    snprintf(progressText, sizeof(progressText), "%d%%", (int)(progress * 100));
    ImGui::ProgressBar(progress, ImVec2(progressBarWidth, 30), progressText);

    ImGui::End();
    // Code highlight window
    if (linkedList.hasFrames()) {
        ImGui::SetNextWindowSizeConstraints(
            ImVec2(250.0f, 200.0f),                      // Minimum size
            ImVec2(800.0f, window.getSize().y - 20.0f)   // Maximum size
        );
        // Dynamically position it based on its current width from the Theme
        ImGui::SetNextWindowPos(ImVec2(window.getSize().x - Theme::CodePanelWidth - 10.0f, 10.0f), ImGuiCond_Always);

        ImGui::SetNextWindowSize(ImVec2(Theme::CodePanelWidth, 300.0f), ImGuiCond_Once);

        ImGui::Begin("Execution Steps", nullptr, ImGuiWindowFlags_NoCollapse);

        Theme::CodePanelWidth = ImGui::GetWindowWidth();

        const auto& frames = linkedList.getFrames();
        const auto& code = linkedList.getPseudoCode();
        int activeLine = frames[currentStep].activeLineOfCode;

        ImGui::TextWrapped("Action: %s", frames[currentStep].explanation.c_str());
        ImGui::Separator();
        ImGui::Spacing();

        for (size_t i = 0; i < code.size(); ++i) {
            if (i == activeLine) {
                // Highlight the active line in yellow
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", code[i].c_str());
            }
            else {
                ImGui::Text("%s", code[i].c_str());
            }
        }
		ImGui::End();
    }
}

void LinkedListScreen::render(sf::RenderWindow& window) {
	window.draw(backgroundSprite);
    if (!linkedList.hasFrames() || currentStep > totalSteps) return;

    // Get the snapshot of the list for the current frame
    const auto& frame = linkedList.getFrames()[currentStep];
    int numNodes = frame.nodes.size();
    if (numNodes == 0) return;

    // Calculate dynamic centering
    float spacing = Theme::NodeWidth + 50.0f; // 50px gap between nodes
    float totalWidth = (numNodes * Theme::NodeWidth) + ((numNodes - 1) * 50.0f);

    // Start drawing from this X position so the list stays centered
    float startX = (window.getSize().x + Theme::ControlPanelWidth) / 2.0f - (totalWidth / 2.0f) + (Theme::NodeWidth / 2.0f);
    float centerY = window.getSize().y / 2.0f;

    // Loop through the state array and draw everything
    for (size_t i = 0; i < frame.nodes.size(); ++i) {
        const auto& nState = frame.nodes[i];
        sf::Vector2f pos(startX + (i * spacing), centerY);

        // Apply highlighting from theme.h
        sf::Color nodeColor = Theme::NodeDefault;
        if (nState.isDeleting) {
            nodeColor = Theme::NodeDelete;
        }
        else if (nState.isTarget) {
            nodeColor = Theme::NodeFound;
        }
        else if (nState.isHighlighted) {
            nodeColor = Theme::NodeHighlight;
        }

        DrawingUtils::drawRectNode(window, pos, std::to_string(nState.data), nodeColor);

        // Draw the arrow to the next node
        if (i < frame.nodes.size() - 1) {
            sf::Vector2f nextPos(startX + ((i + 1) * spacing), centerY);
			//Calculate the edge points so the arrow doesn't draw inside the nodes
            sf::Vector2f arrowStart(pos.x + Theme::NodeWidth / 2.0f, pos.y);
            sf::Vector2f arrowEnd(nextPos.x - Theme::NodeWidth / 2.0f, nextPos.y);
            DrawingUtils::drawArrow(window, arrowStart, arrowEnd, Theme::LinkDefault);
        }
    }
}