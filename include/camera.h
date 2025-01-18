#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;

    // Constructor declaration only
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = 37.0f,
           float pitch = 0.0f);

    glm::mat4 GetViewMatrix();
    void ProcessKeyboard(bool forward, bool backward, bool left, bool right, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset);

private:
    void UpdateCameraVectors();
};

#endif // CAMERA_H
