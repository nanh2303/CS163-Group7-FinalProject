#include "DrawingUtils.h"
#include "theme.h"
#include "AssetManager.h"
#include <cmath>

// Helper function to perfectly center text
static void centerText(sf::Text& textObj, sf::Vector2f centerPos) {
    sf::FloatRect textRect = textObj.getLocalBounds();
    textObj.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    textObj.setPosition(centerPos);
}

void DrawingUtils::drawRectNode(sf::RenderWindow& window, sf::Vector2f centerPos, const std::string& text, sf::Color fillColor) {
    sf::RectangleShape rect(sf::Vector2f(Theme::NodeWidth, Theme::NodeHeight));
    rect.setOrigin(Theme::NodeWidth / 2.0f, Theme::NodeHeight / 2.0f);
    rect.setPosition(centerPos);
    rect.setFillColor(fillColor);
    rect.setOutlineThickness(2.0f);
    rect.setOutlineColor(Theme::TextNormal);

    window.draw(rect);

    sf::Text sfText(text, AssetManager::getInstance().getFont("main_font"), 20);
    sfText.setFillColor(Theme::TextNormal);
    centerText(sfText, centerPos);
    window.draw(sfText);
}

void DrawingUtils::drawCircleNode(sf::RenderWindow& window, sf::Vector2f centerPos, const std::string& text, sf::Color fillColor) {
    sf::CircleShape circle(Theme::NodeRadius);
    circle.setOrigin(Theme::NodeRadius, Theme::NodeRadius);
    circle.setPosition(centerPos);
    circle.setFillColor(fillColor);
    circle.setOutlineThickness(2.0f);
    circle.setOutlineColor(Theme::TextNormal);

    window.draw(circle);

    sf::Text sfText(text, AssetManager::getInstance().getFont("main_font"), 20);
    sfText.setFillColor(Theme::TextNormal);
    centerText(sfText, centerPos);
    window.draw(sfText);
}

void DrawingUtils::drawArrow(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f end, sf::Color color) {
    // Draw the main line
    sf::VertexArray line(sf::Lines, 2);
    line[0].position = start;
    line[0].color = color;
    line[1].position = end;
    line[1].color = color;
    window.draw(line);

    // Calculate arrow head math
    const float PI = 3.14159265f;
    float angle = std::atan2(end.y - start.y, end.x - start.x);

    // Draw the triangle at the end of the arrow
    sf::ConvexShape arrowhead;
    arrowhead.setPointCount(3);
    arrowhead.setPoint(0, end);
    arrowhead.setPoint(1, sf::Vector2f(end.x - Theme::ArrowSize * std::cos(angle - PI / 6),
        end.y - Theme::ArrowSize * std::sin(angle - PI / 6)));
    arrowhead.setPoint(2, sf::Vector2f(end.x - Theme::ArrowSize * std::cos(angle + PI / 6),
        end.y - Theme::ArrowSize * std::sin(angle + PI / 6)));
    arrowhead.setFillColor(color);
    window.draw(arrowhead);
}