#include "assetManager.h"
#include "menuScreen.h"
#include "theme.h"
#include "imgui.h"
#include "linkedListScreen.h"
#include "AVLTreeScreen.h"
#include "hashTableScreen.h"
#include <algorithm>

MenuScreen::MenuScreen(std::function<void(std::unique_ptr<Screen>)> changeScreenCallback)
    : onChangeScreen(std::move(changeScreenCallback)) {
    bgSprite.setTexture(AssetManager::getInstance().getTexture("main_bg"));
}

void MenuScreen::update(sf::RenderWindow& window, sf::Time deltaTime) {
    (void)deltaTime;

    if (bgTexture.getSize().x > 0 && bgTexture.getSize().y > 0) {
        float windowW = static_cast<float>(window.getSize().x);
        float windowH = static_cast<float>(window.getSize().y);
        float texW = static_cast<float>(bgTexture.getSize().x);
        float texH = static_cast<float>(bgTexture.getSize().y);

        float scale = std::max(windowW / texW, windowH / texH);
        bgSprite.setScale(scale, scale);

        float scaledW = texW * scale;
        float scaledH = texH * scale;
        bgSprite.setPosition((windowW - scaledW) * 0.5f, (windowH - scaledH) * 0.5f);
    }

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(window.getSize().x, window.getSize().y));
    ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

    float windowWidth = ImGui::GetWindowSize().x;
    float windowHeight = ImGui::GetWindowSize().y;

    ImGuiIO& io = ImGui::GetIO();
    ImFont* titleFont = io.Fonts->Fonts[1];

    ImGui::PushFont(titleFont);

    const char* title = "Data Structure Visualizer";
    float textWidth = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPos(ImVec2((windowWidth - textWidth) * 0.5f, windowHeight * 0.15f));
    ImGui::Text("%s", title);

    ImGui::PopFont();
    ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f); 
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

    ImVec2 buttonSize(350, 70);
    float buttonStartX = (windowWidth - buttonSize.x) * 0.5f;
    float currentY = windowHeight * 0.30f;
    float spacingY = 25.0f;

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.16f, 0.50f, 0.73f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.16f, 0.50f, 0.73f, 0.4f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.16f, 0.50f, 0.73f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.16f, 0.50f, 0.73f, 1.0f));

    ImGui::SetCursorPos(ImVec2(buttonStartX, currentY));
    // Linked list button here
    if (ImGui::Button("Singly Linked List", buttonSize)) {
		onChangeScreen(std::make_unique<LinkedListScreen>(onChangeScreen));
    }
    ImGui::PopStyleColor(4);
    currentY += buttonSize.y + spacingY;

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.80f, 0.44f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f, 0.80f, 0.44f, 0.4f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.18f, 0.80f, 0.44f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.18f, 0.80f, 0.44f, 1.0f));

    ImGui::SetCursorPos(ImVec2(buttonStartX, currentY));
	// Hash table button here
    if (ImGui::Button("Hash Table Chaining", buttonSize)) {
        onChangeScreen(std::make_unique<HashTableScreen>(onChangeScreen));
    }
    ImGui::PopStyleColor(4);
    currentY += buttonSize.y + spacingY;

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.90f, 0.49f, 0.13f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.90f, 0.49f, 0.13f, 0.4f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.90f, 0.49f, 0.13f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.90f, 0.49f, 0.13f, 1.0f));

    ImGui::SetCursorPos(ImVec2(buttonStartX, currentY));
	// AVL tree button here
    if (ImGui::Button("AVL Tree", buttonSize)) {
        onChangeScreen(std::make_unique<AVLTreeScreen>(onChangeScreen));
    }
    ImGui::PopStyleColor(4);
    currentY += buttonSize.y + spacingY;

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.61f, 0.35f, 0.71f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.61f, 0.35f, 0.71f, 0.4f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.61f, 0.35f, 0.71f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.61f, 0.35f, 0.71f, 1.0f));

    ImGui::SetCursorPos(ImVec2(buttonStartX, currentY));
	// Graph button here
    if (ImGui::Button("Graph", buttonSize)) {}
    ImGui::PopStyleColor(4);

    ImGui::PopStyleVar(2);

    ImGui::End();
}

void MenuScreen::render(sf::RenderWindow& window) {
    window.draw(bgSprite);
}