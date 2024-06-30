#include "badapple.h"

BadApple::BadApple(unsigned int width, unsigned int height, std::string filepath)
    : width(width)
    , height(height)
    , filepath(filepath)
    , currentFrameID(1)
{
}

int mul = 3; // The pixels in the bmp files are 3 bytes

std::vector<glm::vec3> BadApple::GenerateFramePoints()
{
    std::vector<glm::vec3> points;

    if (currentFrameData == nullptr)
    {
        std::cout << "BADAPPLE: frame data not initialized." << std::endl;
        return points;
    }

    glm::ivec2 centering(width / 2, height / 2);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++) {
            unsigned char thisPixel = currentFrameData[(y * width + x) * mul];
            if (thisPixel != UINT8_MAX) {
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

void BadApple::SetCurrentFrame(unsigned int frameID)
{
    currentFrameID = frameID;
}

/*
 * Private functions
 */

unsigned char* BadApple::ReadBMP(unsigned int frameID)
{
    unsigned int size = width * height;
    unsigned char* data = new unsigned char[size * mul];

    std::string thisPath = filepath + '_' + std::to_string(frameID) + ".bmp";
    std::cout << "opening " << thisPath << std::endl;
    FILE* fptr = fopen(thisPath.c_str(), "rb");
    // skip the 54-byte header
    fseek(fptr, 54, SEEK_SET);
    // read image data
    fread(data, sizeof(unsigned char) * mul, size, fptr);
    fclose(fptr);

    return data;
}