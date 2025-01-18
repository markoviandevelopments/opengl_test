#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <ft2build.h>
#include <string>
#include <cmath>
#include FT_FREETYPE_H
#include "shaders.h"

#include <map>


// Structure to hold information about a character
struct Character
{
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

std::map<char, Character> Characters;
unsigned int VAO_text, VBO_text;

void initTextRendering(const char *fontPath)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath, 0, &face))
    {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }

        if (glGetError() != GL_NO_ERROR)
        {
            std::cerr << "OpenGL Error: Failed to generate texture for character: " << c << std::endl;
        }
        std::cout << "Generated texture for character: " << c
                  << ", Width: " << face->glyph->bitmap.width
                  << ", Height: " << face->glyph->bitmap.rows << std::endl;

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)};
        Characters.insert(std::pair<char, Character>(c, character));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO_text);
    glGenBuffers(1, &VBO_text);
    glBindVertexArray(VAO_text);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_text);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderText(unsigned int shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO_text);

    for (auto c : text)
    {
        Character ch = Characters[c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float vertices[6][4] = {
            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos, ypos, 0.0f, 1.0f},
            {xpos + w, ypos, 1.0f, 1.0f},

            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos + w, ypos, 1.0f, 1.0f},
            {xpos + w, ypos + h, 1.0f, 0.0f}};

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_text);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.Advance >> 6) * scale; // Advance is in 1/64th pixels
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int createTextShader(const char *vertexShaderSource, const char *fragmentShaderSource)
{
    unsigned int vertexShader, fragmentShader, shaderProgram;

    // Compile vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // Compile fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // Link shaders into a program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    // Clean up shaders (no longer needed after linking)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

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

    glm::vec3 cameraPos(0.0f, 0.0f, 2.0f);
    glm::vec3 cameraFront(0.0f, -0.5f, -1.0f);
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
    float yaw = 40.0f, pitch = 0.0f;
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
    
    glClearColor(0.2f, 0.3f, 0.5f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        float cameraSpeed = 8.5f * deltaTime;
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
        glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f + sin(currentFrame) * 5.0f, 0.5f, 10.0f + cos(currentFrame) * 5.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(cubeModel));
        glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        std::string x_str = std::to_string(std::round(10.0f * cameraPos[0]) * 0.1f);
        x_str = x_str.substr(0, x_str.find('.') + 2);
        std::string z_str = std::to_string(std::round(10.0f * cameraPos[2]) * 0.1f);
        z_str = z_str.substr(0, z_str.find('.') + 2);

        std::string pitch_str = std::to_string(std::round(10.0f * pitch) * 0.1f);
        pitch_str = pitch_str.substr(0, pitch_str.find('.') + 2);
        std::string yaw_str = std::to_string(std::round(10.0f * yaw) * 0.1f);
        yaw_str = yaw_str.substr(0, yaw_str.find('.') + 2);

        std::string time_str = std::to_string(std::round(10.0f * currentFrame) * 0.1f);
        time_str = time_str.substr(0, time_str.find('.') + 2);


        std::string full_str = "X: " + x_str + "   Z: " + z_str + "   Yaw: " + yaw_str + "   Pitch: " + pitch_str + "   Time: " + time_str;
        

        renderText(textShader, full_str, 25.0f, 550.0f, 0.5f, glm::vec3(1.0f, 0.0f, 1.0f)); // Adjust scale

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

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
        glm::vec3 left = glm::normalize(glm::cross(cameraFront, cameraUp)); // Left is perpendicular to cameraFront and up
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
