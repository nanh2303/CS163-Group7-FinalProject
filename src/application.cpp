#include "application.h"
#include "theme.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include "menuScreen.h"
#include "assetManager.h"

Application::Application()
    : window(sf::VideoMode(1024, 768), "CS163 Group 7 - Data Structure Visualizer") {

    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    ImFont* myFont = io.Fonts->AddFontFromFileTTF("assets/ARLRDBD.TTF", 22.0f);
    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 22.0f; // Keeps the icons scaled with text
    static const ImWchar icon_ranges[] = { 0xf000, 0xf8ff, 0 }; // The Unicode range for FontAwesome
    io.Fonts->AddFontFromFileTTF("assets/Font Awesome 7 Free-Solid-900.otf", 20.0f, &config, icon_ranges);
    ImFont* titleFont = io.Fonts->AddFontFromFileTTF("assets/ARLRDBD.TTF", 44.0f);
    if (myFont != nullptr && titleFont != nullptr) {
        ImGui::SFML::UpdateFontTexture();
    }
    AssetManager::getInstance().loadTexture("main_bg", "assets/constellationBackground.png");
	AssetManager::getInstance().loadFont("main_font", "assets/ARLRDBD.TTF");

    currentScreen = std::make_unique<MenuScreen>([this](std::unique_ptr<Screen> newScreen) {
        changeScreen(std::move(newScreen));
    });
}

Application::~Application() {
    ImGui::SFML::Shutdown();
}

void Application::changeScreen(std::unique_ptr<Screen> newScreen) {
    currentScreen = std::move(newScreen);
}

void Application::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0.0f, 0.0f, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
        }

        sf::Time deltaTime = deltaClock.restart();
        ImGui::SFML::Update(window, deltaTime);

        if (currentScreen) {
            currentScreen->update(window, deltaTime);
        }

        window.clear(Theme::Background);

        if (currentScreen) {
            currentScreen->render(window);
        }

        ImGui::SFML::Render(window);
        window.display();
    }
}