#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <GLFW/glfw3.h>
#include "camera.h"

void processInput(GLFWwindow *window, Camera &camera, float deltaTime, bool &isRunning, bool &isPartyMode);

#endif // INPUT_HANDLER_H
