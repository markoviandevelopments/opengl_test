#include "draw_food_grid.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h> // Include this to resolve glfwGetTime
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

// Constructor
FoodGrid::FoodGrid() : VAO(0), VBO(0) {}

// Destructor
FoodGrid::~FoodGrid()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

// Initialize VAO/VBO
void FoodGrid::initialize()
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

void FoodGrid::drawFoodGrid(unsigned int shaderProgram, const glm::mat4 &view, const glm::mat4 &projection, const std::vector<float>& gameState)
{

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glUseProgram(shaderProgram);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    float maxx = 0;
    int maxIt = 11;
    for (int i=10; i < 73; i++) {
        if (gameState[i] > maxx && gameState[i] < 1000.0f) {
            maxIt = i;
            maxx = gameState[i];
        }
    }
    float maxValue = gameState[maxIt];
    std::cout << maxValue << std::endl;
    int index;
    float intensity;
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "color");

    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            index = row * 8 + col;
            glm::vec3 position(col + 40.0f, 0.0f, row);
            intensity = (maxValue > 0.0f) ? (gameState[10 + index] / maxValue) : 0.0f;
            glm::vec3 color = glm::vec3(intensity, intensity, intensity);


            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(colorLoc, 1, glm::value_ptr(color));

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
}
