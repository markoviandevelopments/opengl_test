#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <sstream>
#include <iomanip>
#include <ft2build.h>
#include <string>
#include <cmath>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>


#include FT_FREETYPE_H

#include "text_renderer.h"
#include "shaders.h"
#include "camera.h"
#include "input_handler.h"
#include "input_handler.h"
#include "cube.h"
#include "player.h"
#include "player2.h"
#include "chessboard.h"

#include <map>

std::map<char, Character> Characters;
unsigned int VAO_text, VBO_text;


std::string formatFloat(float value)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << value;
    return stream.str();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

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

    std::vector<float> gameState;
    glm::vec3 playerPosition(0.0f, 0.0f, 0.0f); // Default player position


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

    Chessboard chessboard;
    chessboard.initialize();

    // Initialize Camera
    Camera camera(glm::vec3(0.0f, 2.0f, 2.0f));

    bool isRunning = false;
    bool isPartyMode = false;

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

    Cube cube;
    Player player;
    Player2 player2;
    

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

        playerPosition.x = camera.Position.x;
        playerPosition.y = camera.Position.y;
        playerPosition.z = camera.Position.z;

        float server_time = 0.0f;
        std::string server_time_str = "0.00";

        // Send player's current position to the server
        std::ostringstream positionStream;
        positionStream << playerPosition.x << "," << playerPosition.y << "," << playerPosition.z;
        std::string positionMessage = positionStream.str();
        if (send(sock, positionMessage.c_str(), positionMessage.length(), 0) < 0) {
            std::cerr << "Error sending player position to server." << std::endl;
            break;
        }

        // Receive information from the server
        int valread = read(sock, buffer, sizeof(buffer));
        if (valread > 0) {
            std::string serverMessage(buffer, valread);
            memset(buffer, 0, sizeof(buffer)); // Clear buffer
            gameState.clear();
            // Parse the server message into a vector of floats
            std::istringstream messageStream(serverMessage);
            std::string value;
            while (std::getline(messageStream, value, ',')) {
                try {
                    gameState.push_back(std::stof(value));
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing value: " << value << " - " << e.what() << std::endl;
                }
            }

            // Print the parsed game state for debugging
            std::cout << "Server time: " << gameState[0] << std::endl;
            std::cout << "Player 1 Position: (" << gameState[1] << ", " << gameState[2] << ", " << gameState[3] << ")" << std::endl;
            std::cout << "Player 2 Position: (" << gameState[4] << ", " << gameState[5] << ", " << gameState[6] << ")" << std::endl;
            if (!gameState.empty()) { // Ensure the vector is not empty
                server_time = gameState[0]; // Get server_time as a float

                // Convert server_time to a string with fixed-point precision
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2) << server_time;
                server_time_str = oss.str();

                // Debug output to verify
                std::cout << "Server time as float: " << server_time << std::endl;
                std::cout << "Server time as string: " << server_time_str << std::endl;
            } else {
                std::cerr << "Error: gameState is empty. Cannot set server_time or server_time_str." << std::endl;
                
            }
        } else {
            std::cerr << "Disconnected from server or error reading data." << std::endl;
            break; // Exit loop if server disconnects
        }

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, camera, deltaTime, isRunning, isPartyMode);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view = camera.GetViewMatrix();

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        if (isPartyMode)
        {
            chessboard.drawDynamicFloor(shaderProgram, view, projection, server_time);
        }
        else
        {
            chessboard.drawChessboard(shaderProgram, view, projection);
        }

        // Draw a green cube on the chessboard at (4, 1, 4)
        cube.draw(shaderProgram, view, projection, server_time);

        player.draw(shaderProgram, view, projection, server_time, camera);

        if (!gameState.empty()) {
            std::cout << "Player 1 x pos: " << gameState[1] << std::endl;
            player2.draw(shaderProgram, view, projection, server_time, gameState[1], gameState[2], gameState[3]);
            player2.draw(shaderProgram, view, projection, server_time, gameState[4], gameState[5], gameState[6]);
        } else {
            std::cout << "EMPTY..." << std::endl;
        }

        // Reset to default (solid) mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);


        // Render Text
        std::string full_str = "X: " + formatFloat(camera.Position.x) +
                               "   Y: " + formatFloat(camera.Position.y) +
                               "   Z: " + formatFloat(camera.Position.z) +
                               "   Yw: " + formatFloat(camera.Yaw) +
                               "   P: " + formatFloat(camera.Pitch) +
                               "   T: " + formatFloat(glfwGetTime());

        renderText(textShader, full_str, 25.0f, 550.0f, 0.5f, glm::vec3(0.8f, 0.8f, 0.0f));
        renderText(textShader, "B: " + formatFloat(server_time), 25.0f, 500.0f, 0.5f, glm::vec3(0.8f, 0.8f, 0.0f));

            glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();

    close(sock);

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
