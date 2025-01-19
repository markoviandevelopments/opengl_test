#include "draw_food_grid.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h> // Include this to resolve glfwGetTime
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

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

void FoodGrid::draw(unsigned int shaderProgram, const glm::mat4 &view, const glm::mat4 &projection)
{
    drawFoodGrid(shaderProgram, view, projection);
}

void FoodGrid::drawFoodGrid(unsigned int shaderProgram, const glm::mat4 &view, const glm::mat4 &projection)
{
    glUseProgram(shaderProgram);

    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            glm::vec3 position(col + 40.0f, 0.0f, row);
            glm::vec3 color = (row + col) % 2 == 0 ? glm::vec3(1.0f) : glm::vec3(0.0f);


            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(color));
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
}
