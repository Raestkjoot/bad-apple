#include "badapple.h"

std::vector<glm::vec3> BadApple::GenerateFramePoints()
{
    std::vector<glm::vec3> points;

    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++) {
            points.push_back(glm::vec3(x, y, 0.0f));
        }
    }

    return points;
}