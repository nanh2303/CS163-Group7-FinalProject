#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class DrawingUtils {
public:
    // Draws a linked list / hash table style rectangular node
    static void drawRectNode(sf::RenderWindow& window, sf::Vector2f centerPos, const std::string& text, sf::Color fillColor);

    // Draws a tree / graph style circular node
    static void drawCircleNode(sf::RenderWindow& window, sf::Vector2f centerPos, const std::string& text, sf::Color fillColor);

    // Draws an arrow pointing from start to end
    static void drawArrow(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f end, sf::Color color);
};