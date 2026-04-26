#include "hashTableScreen.h"
#include "menuScreen.h"
#include "theme.h"
#include "imgui.h"
#include "assetManager.h"
#include "drawingUtils.h"

#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#endif

#ifdef _WIN32
std::string openFileDialog() {
    OPENFILENAMEA ofn;
    CHAR szFile[260] = {0};
    
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0"; // Only show .txt by default
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);
    }
    return ""; // User canceled the dialog
}
#endif

HashTableScreen::HashTableScreen(std::function<void(std::unique_ptr<Screen>)> changeScreenCallback) : onChangeScreen(std::move(changeScreenCallback)) {
    sf::Texture& bgTexture = AssetManager::getInstance().getTexture("main_bg");
    backgroundSprite.setTexture(bgTexture);
    backgroundSprite.setColor(sf::Color(255, 255, 255, 150));
}

void HashTableScreen::startAnimation() {
    if(hashTable.hasFrames()) {
        totalSteps = hashTable.getFrames().size() - 1;
        currentStep = 0;
        isPlaying = true;
        timer = 0.0f;
    }
}

void HashTableScreen::update(sf::RenderWindow& window, sf::Time deltaTime) {
    (void)deltaTime;

    if(isPlaying && totalSteps > 0) {
        timer += deltaTime.asSeconds() * playbackSpeed;
        if(timer >= 1.0f) {
            timer = 0.0f;
            if(currentStep < totalSteps) {
                currentStep++;
            }
            else isPlaying = false;
        }
    }

    // 2. MAIN CONTROL PANEL (Left Side)
    ImGui::SetNextWindowSizeConstraints(
        ImVec2(250.0f, window.getSize().y - 20.0f),
        ImVec2(800.0f, window.getSize().y - 20.0f)
    );
    ImGui::SetNextWindowSize(ImVec2(Theme::ControlPanelWidth, window.getSize().y - 20.0f), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);

    ImGui::Begin("Hash Table Controls", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
    Theme::ControlPanelWidth = ImGui::GetWindowWidth();

    if (ImGui::Button("<- Back to Menu")) {
        onChangeScreen(std::make_unique<MenuScreen>(onChangeScreen));
        ImGui::End();
        return;
    }

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Data operations:");
    // Initialization
    if (ImGui::CollapsingHeader("Initialize", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputInt("Capacity", &tableCapacity);
        if (tableCapacity < 1) tableCapacity = 1; // Prevent div by 0
        
        if (ImGui::Button("Empty Table", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            hashTable.initEmpty(tableCapacity);
            isTableInitialized = true;
            startAnimation();
        }

        ImGui::Spacing();
        
        // --- NEW FILE INIT UI ---
        ImGui::InputText("File Path", filePath, sizeof(filePath));
        // Make the text box take up most of the space, leaving room for the Browse button
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 90.0f); 
        ImGui::InputText("##filepath", filePath, sizeof(filePath)); 
        
        ImGui::SameLine();
        
        if (ImGui::Button("Browse", ImVec2(85.0f, 0))) {
        #ifdef _WIN32
            std::string selectedFile = openFileDialog();
            if (!selectedFile.empty()) {
                // Safely copy the selected path into your ImGui input buffer
                strncpy(filePath, selectedFile.c_str(), sizeof(filePath) - 1);
                filePath[sizeof(filePath) - 1] = '\0'; // Ensure null-termination
            }
        #else
            // Fallback if someone compiles this on Mac/Linux
            strncpy(filePath, "Not supported on this OS", sizeof(filePath));
        #endif
        }
        if (ImGui::Button("Initialize from File", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            hashTable.initFromFile(filePath, tableCapacity);
            isTableInitialized = true;
            startAnimation();
        }
    }
    if(!isTableInitialized) {
        ImGui::BeginDisabled();
    }

    // Insertion
    if (ImGui::CollapsingHeader("Add", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputInt("Add value", &inputValue);
        if (ImGui::Button("Insert Value", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            hashTable.insertValue(inputValue);
            startAnimation();
        }
    }


    // Deletion
    if (ImGui::CollapsingHeader("Delete", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputInt("Delete value", &deleteValue);
        if (ImGui::Button("Remove Value", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            hashTable.deleteValue(deleteValue);
            startAnimation();
        }
    }

    // Search
    if (ImGui::CollapsingHeader("Search", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputInt("Search value", &searchValue);
        if (ImGui::Button("Find Value", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            hashTable.searchValue(searchValue);
            startAnimation();
        }
    }

    // Update
    if (ImGui::CollapsingHeader("Update", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputInt("Old value", &updateOldValue);
        ImGui::InputInt("New value", &updateNewValue);
        
        if (ImGui::Button("Update Value", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            hashTable.updateValue(updateOldValue, updateNewValue);
            startAnimation();
        }
    }

    if(!isTableInitialized) {
        ImGui::EndDisabled();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Theme Settings
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

    // 3. PLAYBACK CONTROL BAR (Bottom)
    float bottomBarHeight = 120.0f;
    float middleCanvasWidth = std::max(10.0f, window.getSize().x - Theme::ControlPanelWidth - Theme::CodePanelWidth);

    ImGui::SetNextWindowPos(ImVec2(Theme::ControlPanelWidth, window.getSize().y - bottomBarHeight));
    ImGui::SetNextWindowSize(ImVec2(middleCanvasWidth, bottomBarHeight));

    ImGui::Begin("Playback controls", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

    ImVec2 btnSize(45, 30);
    float styleSpacing = ImGui::GetStyle().ItemSpacing.x;
    float totalTopRowWidth = (btnSize.x * 4.0f) + 120.0f + 50.0f + (styleSpacing * 4.0f);
    ImGui::SetCursorPosX((middleCanvasWidth - totalTopRowWidth) * 0.5f);

    if (ImGui::Button(u8"\uf048", btnSize)) { isPlaying = false; if (currentStep > 0) currentStep--; }
    ImGui::SameLine();
    
    if (isPlaying) {
        if (ImGui::Button(u8"\uf04c", btnSize)) isPlaying = false;
    } else {
        if (ImGui::Button(u8"\uf04b", btnSize)) isPlaying = true;
    }
    ImGui::SameLine();
    
    if (ImGui::Button(u8"\uf051", btnSize)) { isPlaying = false; if (currentStep < totalSteps) currentStep++; }
    ImGui::SameLine();
    
    if (ImGui::Button(u8"\uf050", btnSize)) { isPlaying = false; currentStep = totalSteps; }
    
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120.0f);
    ImGui::SliderFloat("Speed", &playbackSpeed, 0.5f, 3.0f, "%.1fx");

    ImGui::Spacing(); ImGui::Spacing();

    float progressBarWidth = 400.0f;
    ImGui::SetCursorPosX((middleCanvasWidth - progressBarWidth) * 0.5f);
    float progress = (totalSteps > 0) ? (float)currentStep / (float)totalSteps : 0.0f;
    char progressText[32];
    snprintf(progressText, sizeof(progressText), "%d%%", (int)(progress * 100));
    ImGui::ProgressBar(progress, ImVec2(progressBarWidth, 30), progressText);

    ImGui::End();

    // 4. CODE HIGHLIGHT WINDOW (Right Side)
    if (hashTable.hasFrames()) {
        ImGui::SetNextWindowSizeConstraints(
            ImVec2(250.0f, 200.0f), 
            ImVec2(800.0f, window.getSize().y - 20.0f)
        );
        ImGui::SetNextWindowPos(ImVec2(window.getSize().x - Theme::CodePanelWidth - 10.0f, 10.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(Theme::CodePanelWidth, 300.0f), ImGuiCond_Once);

        ImGui::Begin("Execution Steps", nullptr, ImGuiWindowFlags_NoCollapse);
        Theme::CodePanelWidth = ImGui::GetWindowWidth();

        const auto& frames = hashTable.getFrames();
        const auto& code = hashTable.getPseudoCode();
        int activeLine = frames[currentStep].activeLineOfCode;

        ImGui::TextWrapped("Action: %s", frames[currentStep].explanation.c_str());
        ImGui::Separator();
        ImGui::Spacing();

        for (size_t i = 0; i < code.size(); ++i) {
            if (i == activeLine) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", code[i].c_str());
            } else {
                ImGui::Text("%s", code[i].c_str());
            }
        }
        ImGui::End();
    }
}

void HashTableScreen::render(sf::RenderWindow& window) {
    window.draw(backgroundSprite);
    if (!hashTable.hasFrames() || currentStep > totalSteps) return;

    const auto& frame = hashTable.getFrames()[currentStep];
    int capacity = frame.table.size();
    if (capacity == 0) return;

    // Layout configuration
    float verticalSpacing = Theme::NodeHeight + 20.0f; // Gap between array slots
    float horizontalSpacing = Theme::NodeWidth + 40.0f; // Gap between chained nodes
    
    // Center the table vertically in the window
    float totalHeight = (capacity * Theme::NodeHeight) + ((capacity - 1) * 20.0f);
    float startY = (window.getSize().y / 2.0f) - (totalHeight / 2.0f) + (Theme::NodeHeight / 2.0f);
    
    // Position the backbone array on the left side of the visualizer area
    // Offset slightly from the control panel
    float startX = Theme::ControlPanelWidth + 80.0f + (Theme::NodeWidth / 2.0f); 

    for (int i = 0; i < capacity; ++i) {
        sf::Vector2f arrayBoxPos(startX, startY + (i * verticalSpacing));

        // Highlight the active array bucket index
        sf::Color boxColor = (frame.activeBucketIndex == i) ? Theme::NodeHighlight : Theme::NodeDefault;
        
        // Draw the Array Index Box (The "Backbone")
        DrawingUtils::drawRectNode(window, arrayBoxPos, "[" + std::to_string(i) + "]", boxColor);

        // Draw the chained nodes extending to the right
        const auto& bucket = frame.table[i];
        for (size_t j = 0; j < bucket.size(); ++j) {
            const auto& nState = bucket[j];
            
            // X position shifts right based on how deep we are in the chain
            sf::Vector2f nodePos(startX + ((j + 1) * horizontalSpacing), arrayBoxPos.y);

            sf::Color nodeColor = Theme::NodeDefault;
            if (nState.isDeleting) nodeColor = Theme::NodeDelete;
            else if (nState.isTarget) nodeColor = Theme::NodeFound;
            else if (nState.isHighlighted) nodeColor = Theme::NodeHighlight;

            DrawingUtils::drawRectNode(window, nodePos, std::to_string(nState.data), nodeColor);

            // Draw arrow connecting the previous box/node to this node
            sf::Vector2f prevPos = (j == 0) ? arrayBoxPos : sf::Vector2f(startX + (j * horizontalSpacing), arrayBoxPos.y);
            
            sf::Vector2f arrowStart(prevPos.x + Theme::NodeWidth / 2.0f, prevPos.y);
            sf::Vector2f arrowEnd(nodePos.x - Theme::NodeWidth / 2.0f, nodePos.y);
            
            DrawingUtils::drawArrow(window, arrowStart, arrowEnd, Theme::LinkDefault);
        }
    }
}