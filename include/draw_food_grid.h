#ifndef FOODGRID_H
#define FOODGRID_H

#include <glm/glm.hpp>
#include <vector>

class FoodGrid
{
private:
    unsigned int VAO, VBO;

public:
    FoodGrid();
    ~FoodGrid();

    void initialize(); // Explicit initialization
    void drawFoodGrid(unsigned int shaderProgram, const glm::mat4 &view, const glm::mat4 &projection, const std::vector<float>& gameState);
};

#endif // FOODGRID_H
