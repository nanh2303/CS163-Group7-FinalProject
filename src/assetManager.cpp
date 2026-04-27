#include "core/assetManager.h"
#include <iostream>

void AssetManager::loadTexture(std::string name, std::string fileName) {
    sf::Texture tex;
    if (tex.loadFromFile(fileName)) {
        // Enable smoothing and generate mipmaps so it looks sharp when scaled!
        tex.setSmooth(true);
        tex.generateMipmap();
        // Insert it into our map
        textures[name] = tex;
    }
    else {
        std::cerr << "Error: Could not load texture: " << fileName << std::endl;
    }
}

sf::Texture& AssetManager::getTexture(std::string name) {
    return textures.at(name); // Throws an error if the texture name doesn't exist
}

void AssetManager::loadFont(std::string name, std::string fileName) {
    sf::Font font;
    if (font.loadFromFile(fileName)) {
        fonts[name] = font;
    }
    else {
        std::cerr << "Error: Could not load font " << fileName << std::endl;
    }
}

sf::Font& AssetManager::getFont(std::string name) {
    return fonts.at(name);
}