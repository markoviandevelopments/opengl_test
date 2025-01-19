#include "chessboard.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h> // Include this to resolve glfwGetTime
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

// Constructor
Chessboard::Chessboard() : VAO(0), VBO(0) {}

// Destructor
Chessboard::~Chessboard()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

// Initialize VAO/VBO
void Chessboard::initialize()
{
    float vertices[] = {
        -0.5f, 0.0f, -0.5f,
        0.5f, 0.0f, -0.5f,
        0.5f, 0.0f, 0.5f,
        -0.5f, 0.0f, -0.5f,
        0.5f, 0.0f, 0.5f,
        -0.5f, 0.0f, 0.5f};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Chessboard::draw(unsigned int shaderProgram, const glm::mat4 &view, const glm::mat4 &projection, bool isPartyMode, float server_time)
{
    if (isPartyMode)
    {
        drawDynamicFloor(shaderProgram, view, projection, server_time);
    }
    else
    {
        drawChessboard(shaderProgram, view, projection);
    }
}

void Chessboard::drawChessboard(unsigned int shaderProgram, const glm::mat4 &view, const glm::mat4 &projection)
{
    glUseProgram(shaderProgram);

    for (int row = 0; row < 32; ++row)
    {
        for (int col = 0; col < 32; ++col)
        {
            glm::vec3 position(col, 0.0f, row);
            glm::vec3 color = (row + col) % 2 == 0 ? glm::vec3(1.0f) : glm::vec3(0.0f);

            if (color[0] < 0.5f)
            {
                color = glm::vec3(sin((5.0f * sin(row) + col) * 0.4f), 0.0f, 0.0f);
            }

            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(color));
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
}

void Chessboard::drawDynamicFloor(unsigned int shaderProgram, const glm::mat4 &view, const glm::mat4 &projection, float server_time)
{
    glUseProgram(shaderProgram);

    float time = server_time;
    for (int row = 0; row < 32; ++row)
    {
        for (int col = 0; col < 32; ++col)
        {
            glm::vec3 position(col, 0.0f, row);
            glm::vec3 color = glm::vec3((sin(time + row * 0.1f) + 1.0f) / 2.0f, (cos(time + col * 0.1f) + 1.0f) / 2.0f, 0.5f);

            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(color));

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
}
