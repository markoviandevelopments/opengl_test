#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <ft2build.h>
#include <string>
#include <cmath>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


#include FT_FREETYPE_H

#include "text_renderer.h"
#include "shaders.h"
#include "camera.h"

#include <map>

std::map<char, Character> Characters;
unsigned int VAO_text, VBO_text;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, Camera &camera, float deltaTime);

#define PORT 12346

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Chessboard World", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Connect to Server
    int sock = 0;
    struct sockaddr_in serv_addr;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and connect
    if (inet_pton(AF_INET, "50.188.120.138", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    char buffer[1024] = {0};



    // Initialize Camera
    Camera camera(glm::vec3(0.0f, 2.0f, 2.0f));

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

    // Define cube vertices and indices
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

    unsigned int cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // Initialize text rendering
    initTextRendering("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    unsigned int textShader = createTextShader(textVertexShaderSource, textFragmentShaderSource);

    // Set orthographic projection based on the current window size
    glm::mat4 textProjection = glm::ortho(0.0f, static_cast<float>(width),
                                          0.0f, static_cast<float>(height));
    glUseProgram(textShader);
    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, glm::value_ptr(textProjection));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    glClearColor(0.2f, 0.3f, 0.5f, 1.0f);

    while (!glfwWindowShouldClose(window)) {

        // Get information from Server
        std::string server_time_str;
        int valread = read(sock, buffer, 1024);
        if (valread > 0) {
            //std::cout << buffer;
            server_time_str = "";
            server_time_str = buffer;
            memset(buffer, 0, sizeof(buffer)); // Clear buffer
        } else {
            break; // Exit loop if server disconnects
        }

        float server_time = std::stof(server_time_str);


        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, camera, deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view = camera.GetViewMatrix();

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        for (int row = 0; row < 32; ++row) {
            for (int col = 0; col < 32; ++col) {
                glm::vec3 position(col, 0.0f, row);
                glm::vec3 color = (row + col) % 2 == 0 ? glm::vec3(1.0f) : glm::vec3(0.0f);

                if (color[0] < 0.5f) {
                    color = glm::vec3(sin((5.0f * sin(row) + col) * 0.4f), 0.0f, 0.0f);
                }

                glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

                glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(color));
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        // Draw a green cube on the chessboard at (4, 1, 4)
        glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f + sin(server_time * 0.01f) * 5.0f, 0.5f, 10.0f + cos(server_time * 0.01f) * 5.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(cubeModel));
        glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Draw the wireframe outline
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
        glLineWidth(2.0f); // Optional: Set outline thickness
        glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f))); // Red outline
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Reset to default (solid) mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);


        // Render Text
        std::string x_str = std::to_string(std::round(10.0f * camera.Position.x) * 0.1f);
        x_str = x_str.substr(0, x_str.find('.') + 2);
        std::string y_str = std::to_string(std::round(10.0f * camera.Position.y) * 0.1f);
        y_str = y_str.substr(0, y_str.find('.') + 2);
        std::string z_str = std::to_string(std::round(10.0f * camera.Position.z) * 0.1f);
        z_str = z_str.substr(0, z_str.find('.') + 2);

        std::string pitch_str = std::to_string(std::round(10.0f * camera.Pitch) * 0.1f);
        pitch_str = pitch_str.substr(0, pitch_str.find('.') + 2);
        std::string yaw_str = std::to_string(std::round(10.0f * camera.Yaw) * 0.1f);
        yaw_str = yaw_str.substr(0, yaw_str.find('.') + 2);

        std::string time_str = std::to_string(std::round(10.0f * glfwGetTime()) * 0.1f);
        time_str = time_str.substr(0, time_str.find('.') + 2);

        std::string full_str = "X: " + x_str + "   Y: " + y_str + "   Z: " + z_str + "   Yw: " + yaw_str + "   P: " + pitch_str + "   T: " + time_str;

        renderText(textShader, full_str, 25.0f, 550.0f, 0.5f, glm::vec3(1.0f, 0.0f, 1.0f));

        full_str = "B: " + server_time_str;
        renderText(textShader, full_str, 25.0f, 500.0f, 0.5f, glm::vec3(1.0f, 0.0f, 1.0f));


        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    glfwTerminate();

    close(sock);

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, Camera &camera, float deltaTime)
{
    // WASD for translation
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(true, false, false, false, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(false, true, false, false, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(false, false, true, false, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(false, false, false, true, deltaTime);

    // Arrow keys for rotation
    float sensitivity = 50.0f * deltaTime; // Adjust sensitivity
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessMouseMovement(0.0f, sensitivity);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessMouseMovement(0.0f, -sensitivity);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessMouseMovement(-sensitivity, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessMouseMovement(sensitivity, 0.0f);
}
