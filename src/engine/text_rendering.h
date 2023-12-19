#pragma once

#include <iostream>
#include <map>

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    signed long Advance;    // Offset to advance to next glyph
};

struct Font {
    std::map<char, Character> characters;
};

int loadFont(std::string fontPath, Font& font);