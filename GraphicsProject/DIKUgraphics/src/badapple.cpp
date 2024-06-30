#include "badapple.h"

BadApple::BadApple(unsigned int rows, unsigned int cols, std::string filepath)
    : rows(rows)
    , cols(cols)
    , filepath(filepath)
    , currentFrameID(0)
{
}

std::vector<glm::vec3> BadApple::GenerateFramePoints()
{
    std::vector<glm::vec3> points;

    if (currentFrameData == nullptr)
    {
        std::cout << "BADAPPLE: frame data not initialized." << std::endl;
        return points;
    }

    glm::ivec2 centering(rows / 2, cols / 2);

    for (int x = 0; x < rows; x++)
    {
        for (int y = 0; y < cols; y++) {
            if (currentFrameData[y * rows + x] == 'x') {
                points.push_back(glm::vec3(x-centering.x, y-centering.y, 0.0f));
            }
        }
    }

    return points;
}

void BadApple::ReadFrameAndIncrement()
{
    currentFrameData = ReadBMP(currentFrameID);
    currentFrameID++;
}

void BadApple::SetFilepath(const std::string& filepath)
{
    std::cout << "Setting new filepath: " << filepath << std::endl;
    this->filepath = filepath;
}

/*
 * Private functions
 */

unsigned char* BadApple::ReadBMP(unsigned int frameID)
{
    unsigned int size = rows * cols;
    // TODO read from filepath + '_' + frameID.tostring()

    // TODO: Do we need new here, since it's an array?
    unsigned char* data = new unsigned char[size];

    for (unsigned int i = 0; i < size; i++)
    {
        data[i] = 'x';
    }

    return data;
}