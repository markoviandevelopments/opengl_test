#ifndef AGENTS_H
#define AGENTS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



class Agents{
public:
    Agents();  // Constructor to initialize Player data
    ~Agents(); // Destructor to clean up resources

    void draw(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection, float serverTime, float x, float y, float z);

private:
    unsigned int VAO, VBO, EBO; // OpenGL buffers
    void initialize();          // Helper to initialize the player
};

#endif // AGENTS_H
