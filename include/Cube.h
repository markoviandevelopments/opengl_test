#ifndef CUBE_H
#define CUBE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Cube {
public:
    Cube();  // Constructor to initialize cube data
    ~Cube(); // Destructor to clean up resources

    void draw(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection, float serverTime);

private:
    unsigned int VAO, VBO, EBO; // OpenGL buffers
    void initialize();          // Helper to initialize the cube
};

#endif // CUBE_H
