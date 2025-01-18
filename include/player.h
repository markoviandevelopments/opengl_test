#ifndef PLAYER_H
#define PLAYER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"

class Player {
public:
    Player();  // Constructor to initialize Player data
    ~Player(); // Destructor to clean up resources

    void draw(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection, float serverTime, const Camera& camera);

private:
    unsigned int VAO, VBO, EBO; // OpenGL buffers
    void initialize();          // Helper to initialize the player
};

#endif // PLAYER_H
