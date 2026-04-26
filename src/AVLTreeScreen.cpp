#include "AVLTreeScreen.h"
#include "menuScreen.h"
#include "theme.h"
#include "imgui.h"
#include "assetManager.h"
#include "drawingUtils.h"
#include <iostream>

AVLTreeScreen::AVLTreeScreen(std::function<void(std::unique_ptr<Screen>)> changeScreenCallback)
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

void AVLTreeScreen::startAnimation() {
    const auto& frames = avlTree.getAnimation();

    currentStep = 0;
    totalSteps = 0;
    isPlaying = false;
    elapsed = 0.0f;

    if (frames.empty()) {
        return;
    }

    totalSteps = (int)frames.size() - 1;

    if (totalSteps < 0) totalSteps = 0;

    currentStep = 0;
    isPlaying = true;
    elapsed = 0.0f;
}

float AVLTreeScreen::ease(float t) {
    return t * t * (3.0f - 2.0f * t); // smoothstep
}

AVLTree::NodeState AVLTreeScreen::lerpNode(
    const AVLTree::NodeState& a,
    const AVLTree::NodeState& b,
    float t
) {
    AVLTree::NodeState r = a;

    r.x = a.x + (b.x - a.x) * t;
    r.y = a.y + (b.y - a.y) * t;

    r.data = b.data;
    r.left = b.left;
    r.right = b.right;

    r.isHighlighted = b.isHighlighted;
    r.isRotating = b.isRotating;
    r.isDeleting = b.isDeleting;

    return r;
}

void AVLTreeScreen::update(sf::RenderWindow& window, sf::Time deltaTime) {
    if (isPlaying && totalSteps >= 0) {
        elapsed += deltaTime.asSeconds() * playbackSpeed;

        if (elapsed >= transitionTime) {
            elapsed = 0.0f;

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
    ImGui::Begin("AVL Tree controls", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

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
    ImGui::Text("AVL Tree operations:");
    // Initialization
    if (ImGui::CollapsingHeader("Initialize", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("Empty list", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            avlTree.initEmpty();
            currentStep = 0;
            totalSteps = 0;
            isPlaying = false;
        }

        ImGui::Spacing();
        ImGui::InputInt("Random count", &randomCount);
        if (ImGui::Button("Generate random", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            avlTree.initRandom(randomCount);
            
            const auto& frames = avlTree.getAnimation();
            if (!frames.empty()) {
                currentStep = 0;
                totalSteps = (int)frames.size() - 1;
                elapsed = 0.0f;
                isPlaying = true;
            }
        }

        //Input From file
        ImGui::Spacing();

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputText("File Path", filepathBuffer, IM_ARRAYSIZE(filepathBuffer));

        if (ImGui::Button("Load from file", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            if (strlen(filepathBuffer) == 0) {
                std::cout << "Please enter file path\n";
            } else {
                avlTree.initFromFile(filepathBuffer);
                startAnimation();
            }
        }
    }

    // Insertion
    if (ImGui::CollapsingHeader("Add", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputInt("Insert value", &inputValue);
        if (ImGui::Button("Insert##btn_insert", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            avlTree.clearAnimation();
            avlTree.insert(inputValue);
            startAnimation();
        }
    }

    // Deletion
    if (ImGui::CollapsingHeader("Delete", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputInt("Delete value", &deleteValue);
        if (ImGui::Button("Delete##btn_delete", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            avlTree.clearAnimation();
            avlTree.remove(deleteValue);
            startAnimation();
        }
    }

    // Search
    if (ImGui::CollapsingHeader("Search", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputInt("Search value", &searchVal);
        if (ImGui::Button("Search##btn_search", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            avlTree.clearAnimation();
            avlTree.search(searchVal);
            startAnimation();
        }
    }

    // UPDATE
    if (ImGui::CollapsingHeader("Update", ImGuiTreeNodeFlags_DefaultOpen)) {
        static int oldValue = 0;
        static int newValue = 0;

        ImGui::InputInt("Old value", &oldValue);
        ImGui::InputInt("New value", &newValue);

        if (ImGui::Button("Update##btn_update", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            avlTree.clearAnimation();
            avlTree.update(oldValue, newValue);
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

    ImGui::Begin("Playback controls", nullptr, 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoMove
    );

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
    if (avlTree.hasFrames()) {
        ImGui::SetNextWindowSizeConstraints(
            ImVec2(250.0f, 200.0f),                      // Minimum size
            ImVec2(800.0f, window.getSize().y - 20.0f)   // Maximum size
        );
        // Dynamically position it based on its current width from the Theme
        ImGui::SetNextWindowPos(ImVec2(window.getSize().x - Theme::CodePanelWidth - 10.0f, 10.0f), ImGuiCond_Always);

        ImGui::SetNextWindowSize(ImVec2(Theme::CodePanelWidth, 300.0f), ImGuiCond_Once);

        ImGui::Begin("Execution Steps", nullptr, ImGuiWindowFlags_NoCollapse);

        Theme::CodePanelWidth = ImGui::GetWindowWidth();

        const auto& frames = avlTree.getAnimation();
        if (frames.empty()) {
            ImGui::End();
            return;
        }

        int idx = std::min(currentStep, (int)frames.size() - 1);
        int activeLine = frames[idx].activeLineOfCode;

        ImGui::TextWrapped("Action: %s", frames[idx].explanation.c_str());
        ImGui::Separator();
        ImGui::Spacing();

        const auto& code = avlTree.getPseudoCode();
        for (size_t i = 0; i < code.size(); ++i) {
            if ((int)i == activeLine) {
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

void AVLTreeScreen::render(sf::RenderWindow& window) {
    window.draw(backgroundSprite);

    const auto& frames = avlTree.getAnimation();
    if (frames.empty()) return;

    int maxIdx = (int)frames.size() - 1;
    int j = std::clamp(currentStep, 0, maxIdx);

    const auto& B = frames[j];
    std::vector<AVLTree::NodeState> interpolated = B.nodes;

    if (interpolated.empty()) return;

    float leftBound = Theme::ControlPanelWidth;
    float rightBound = window.getSize().x - Theme::CodePanelWidth;

    float centerX = (leftBound + rightBound) / 2.0f;
    float startY = 120.0f;
    float offset = (rightBound - leftBound) / 4.0f;

    std::function<void(int, float, float, float)> layout =
        [&](int idx, float x, float y, float offset) {

        if (idx < 0 || idx >= (int)interpolated.size()) return;

        interpolated[idx].x = x;
        interpolated[idx].y = y;

        int l = interpolated[idx].left;
        int r = interpolated[idx].right;

        if (l >= 0) layout(l, x - offset, y + 100, offset / 2);
        if (r >= 0) layout(r, x + offset, y + 100, offset / 2);
    };

    int rootIdx = B.rootIndex;

    if (rootIdx < 0 || rootIdx >= (int)interpolated.size()) {
        std::vector<bool> hasParent(interpolated.size(), false);

        for (auto& n : interpolated) {
            if (n.left != -1) hasParent[n.left] = true;
            if (n.right != -1) hasParent[n.right] = true;
        }

        for (int i = 0; i < (int)interpolated.size(); i++) {
            if (!hasParent[i]) {
                rootIdx = i;
                break;
            }
        }
    }

    if (rootIdx < 0 || rootIdx >= (int)interpolated.size()) {
        rootIdx = 0;
    }

    layout(rootIdx, centerX, startY, offset);
    drawFromFrame(window, interpolated, rootIdx, 0, 0, 0);
}

void AVLTreeScreen::drawFromFrame(sf::RenderWindow& window,
    const std::vector<AVLTree::NodeState>& nodes,
    int idx, float, float, float) {

    if (idx < 0 || idx >= (int)nodes.size()) return;

    const auto& node = nodes[idx];
    sf::Color color = Theme::NodeDefault;

    if (node.isFound) color = sf::Color::Green;  
    else if (node.isHighlighted) color = sf::Color::Yellow;
    else if (node.isRotating) color = sf::Color::Red;
    else if (node.isDeleting) color = sf::Color::Magenta;

    DrawingUtils::drawRectNode(
        window,
        { node.x, node.y },
        std::to_string(node.data),
        color
    );

    //  LEFT
    if (node.left >= 0 && node.left < (int)nodes.size()) {
        const auto& leftNode = nodes[node.left];

        DrawingUtils::drawArrow(window,
            { node.x, node.y + Theme::NodeHeight / 2 },
            { leftNode.x, leftNode.y - Theme::NodeHeight / 2 },
            Theme::LinkDefault
        );

        drawFromFrame(window, nodes, node.left, 0, 0, 0);
    }

    //  RIGHT
    if (node.right >= 0 && node.right < (int)nodes.size()) {
        const auto& rightNode = nodes[node.right];

        DrawingUtils::drawArrow(window,
            { node.x, node.y + Theme::NodeHeight / 2 },
            { rightNode.x, rightNode.y - Theme::NodeHeight / 2 },
            Theme::LinkDefault
        );

        drawFromFrame(window, nodes, node.right, 0, 0, 0);
    }
}