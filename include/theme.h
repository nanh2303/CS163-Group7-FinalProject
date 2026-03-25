#pragma once
#include <SFML/Graphics/Color.hpp>

namespace Theme {
// COLORS
    // Backgrounds
    inline sf::Color Background(15, 15, 20);         // Very dark gray/blue for empty spaces

    // Node states (The actual data shapes)
    inline sf::Color NodeDefault(41, 128, 185);      // Standard Blue
    inline sf::Color NodeHighlight(243, 156, 18);    // Orange (Used when searching/traversing)
    inline sf::Color NodeFound(46, 204, 113);        // Green (Target found or newly inserted)
    inline sf::Color NodeDelete(231, 76, 60);        // Red (Target to be deleted)

    // Links & text
    inline sf::Color LinkDefault(189, 195, 199);     // Light gray for arrows and lines
    inline sf::Color TextNormal(255, 255, 255);      // White for standard text
    inline sf::Color TextDark(44, 62, 80);           // Dark blue/gray for text inside light nodes

// SIZE
    // Nodes
    inline float NodeRadius = 25.0f;          // For circular nodes (Trees, graphs)
    inline float NodeWidth = 80.0f;           // For rectangular nodes (Linked list, hash table)
    inline float NodeHeight = 50.0f;

    // Links (Lines/Arrows)
    inline float LinkThickness = 3.0f;        // Thickness of the connecting lines
    inline float ArrowSize = 12.0f;           // Size of the triangle at the end of the pointer

// LAYOUT CONSTANT
    inline float ControlPanelWidth = 400.0f;  // Everyone's left-side UI panel must be exactly this wide
}