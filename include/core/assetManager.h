#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

class AssetManager {
public:
    // Get the single, global instance of the manager
    static AssetManager& getInstance() {
        static AssetManager instance;
        return instance;
    }

    // Load a texture into memory from a file
    void loadTexture(std::string name, std::string fileName);

    // Get a reference to that loaded texture
    sf::Texture& getTexture(std::string name);

    //Font
	void loadFont(std::string name, std::string fileName);
	sf::Font& getFont(std::string name);

private:
    AssetManager() = default;

    // Disable copying
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    // The actual storage for the textures (Maps a name string to the actual texture)
    std::map<std::string, sf::Texture> textures;

	// The actual storage for the fonts (Maps a name string to the actual font)
	std::map<std::string, sf::Font> fonts;
};