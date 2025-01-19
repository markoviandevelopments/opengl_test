#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <glm/glm.hpp>

class Chessboard
{
private:
    unsigned int VAO, VBO;

public:
    Chessboard();
    ~Chessboard();

    void initialize(); // Explicit initialization
    void draw(unsigned int shaderProgram, const glm::mat4 &view, const glm::mat4 &projection, bool isPartyMode, float server_time);
    void drawChessboard(unsigned int shaderProgram, const glm::mat4 &view, const glm::mat4 &projection);
    void drawDynamicFloor(unsigned int shaderProgram, const glm::mat4 &view, const glm::mat4 &projection, float server_time);
};

#endif // CHESSBOARD_H
