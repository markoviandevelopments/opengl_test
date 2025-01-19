#ifndef FOODGRID_H
#define FOODGRID_H

#include <glm/glm.hpp>

class FoodGrid
{
private:
    unsigned int VAO, VBO;

public:
    FoodGrid();
    ~FoodGrid();

    void initialize(); // Explicit initialization
    void draw(unsigned int shaderProgram, const glm::mat4 &view, const glm::mat4 &projection);
    void drawFoodGrid(unsigned int shaderProgram, const glm::mat4 &view, const glm::mat4 &projection);
};

#endif // FOODGRID_H
