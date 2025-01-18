#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(8.0f), MouseSensitivity(3.0f)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(bool forward, bool backward, bool left, bool right, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;

    if (forward)
        Position += glm::normalize(glm::vec3(Front.x, 0.0f, Front.z)) * velocity;
    if (backward)
        Position -= glm::normalize(glm::vec3(Front.x, 0.0f, Front.z)) * velocity;
    if (left)
        Position -= glm::normalize(glm::cross(Front, WorldUp)) * velocity;
    if (right)
        Position += glm::normalize(glm::cross(Front, WorldUp)) * velocity;

    // Restrict camera to a specific height (e.g., y = 2.0)
    Position.y = 2.0f;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Clamp pitch to avoid flipping
    Pitch = glm::clamp(Pitch, -89.0f, 89.0f);

    UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
