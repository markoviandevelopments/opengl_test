#include "input_handler.h"
#include "camera.h"     // For Camera class
#include <GLFW/glfw3.h> // For GLFWwindow
#include <iostream>     // For std::cout and std::endl

void processInput(GLFWwindow *window, Camera &camera, float deltaTime, bool &isRunning, bool &isPartyMode)
{
    // Detect "run" state
    isRunning = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    float speedMultiplier = isRunning ? 3.0f : 1.0f; // 3x speed if running

    // WASD for translation
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(true, false, false, false, deltaTime * speedMultiplier);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(false, true, false, false, deltaTime * speedMultiplier);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(false, false, true, false, deltaTime * speedMultiplier);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(false, false, false, true, deltaTime * speedMultiplier);

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

    // Toggle party mode with 'P' key
    static bool pKeyPressedLastFrame = false;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pKeyPressedLastFrame)
    {
        isPartyMode = !isPartyMode; // Toggle party mode
        if (isPartyMode)
            std::cout << "Party mode activated!" << std::endl;
        else
            std::cout << "Party mode deactivated!" << std::endl;
    }
    pKeyPressedLastFrame = glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;
}
