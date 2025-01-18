#include "cube.h"
#include <iostream>

Cube::Cube() {
    initialize();
}

Cube::~Cube() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Cube::initialize() {
    float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
    };

    unsigned int cubeIndices[] = {
        0, 1, 2, 2, 3, 0, // Back face
        4, 5, 6, 6, 7, 4, // Front face
        4, 5, 1, 1, 0, 4, // Bottom face
        7, 6, 2, 2, 3, 7, // Top face
        4, 7, 3, 3, 0, 4, // Left face
        5, 6, 2, 2, 1, 5  // Right face
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // Unbind VAO
}

void Cube::draw(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection, float serverTime) {
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f + sin(serverTime * 0.01f) * 5.0f, 0.5f, 10.0f + cos(serverTime * 0.01f) * 5.0f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Draw the wireframe outline
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
    glLineWidth(2.0f); // Optional: Set outline thickness
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(glm::vec3(0.1f, 0.0f, 1.0f))); // Red outline
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
