#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Vertex Shader
const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 ourColor;

uniform vec3 color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = color;
}
)";

// Fragment Shader
const char *fragmentShaderSource = R"(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(ourColor, 1.0);
}
)";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, float &cameraSpeed, glm::vec3 &cameraPos, glm::vec3 &cameraFront, glm::vec3 &cameraUp, float &yaw, float &pitch);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Chessboard World", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Compile shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Define square vertices
    float vertices[] = {
        -0.5f, 0.0f, -0.5f,
         0.5f, 0.0f, -0.5f,
         0.5f, 0.0f,  0.5f,
        -0.5f, 0.0f, -0.5f,
         0.5f, 0.0f,  0.5f,
        -0.5f, 0.0f,  0.5f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glm::vec3 cameraPos(0.0f, 0.0f, 2.0f);
    glm::vec3 cameraFront(0.0f, -0.5f, -1.0f);
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
    float yaw = 40.0f, pitch = 0.0f;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        float cameraSpeed = 2.5f * deltaTime;
        processInput(window, cameraSpeed, cameraPos, cameraFront, cameraUp, yaw, pitch);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Camera matrices
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        for (int row = 0; row < 32; ++row) {
            for (int col = 0; col < 32; ++col) {
                glm::vec3 position(col, 0.0f, row);
                glm::vec3 color = (row + col) % 2 == 0 ? glm::vec3(1.0f) : glm::vec3(0.0f);

                glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

                glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(color));
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, float &cameraSpeed, glm::vec3 &cameraPos, glm::vec3 &cameraFront, glm::vec3 &cameraUp, float &yaw, float &pitch) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 forward = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z)); // Restrict to xz-plane
        cameraPos += cameraSpeed * forward;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 backward = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z)); // Restrict to xz-plane
        cameraPos -= cameraSpeed * backward;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 left = glm::normalize(glm::cross(cameraUp, cameraFront)); // Left is perpendicular to cameraFront and up
        cameraPos -= cameraSpeed * glm::vec3(left.x, 0.0f, left.z); // Restrict to xz-plane
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp)); // Right is perpendicular to cameraFront and up
        cameraPos += cameraSpeed * glm::vec3(right.x, 0.0f, right.z); // Restrict to xz-plane
    }

    // Ensure camera stays on the surface
    cameraPos.y = 2.0f;

    float sensitivity = 1.5f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        pitch += sensitivity;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        pitch -= sensitivity;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        yaw -= sensitivity;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        yaw += sensitivity;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}
