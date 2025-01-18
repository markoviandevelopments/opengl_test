#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <string>
#include <glm/glm.hpp>
#include <map>

// Structure to hold information about a character
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

// Global variables for text rendering
extern std::map<char, Character> Characters;
extern unsigned int VAO_text, VBO_text;

// Function declarations
void initTextRendering(const char* fontPath);
void renderText(unsigned int shader, std::string text, float x, float y, float scale, glm::vec3 color);
unsigned int createTextShader(const char* vertexShaderPath, const char* fragmentShaderPath);

#endif
