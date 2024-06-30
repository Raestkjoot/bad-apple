#pragma once

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>

#include "traceinfo.h"
#include "glmutils.h"


/**
 * \class BadApple
 * A class which reads a frame from an image and generates vec3 points that can be drawn with OpenGL
 */
class BadApple {
public:

	BadApple(unsigned int width, unsigned int height, std::string filepath);

	/**
	 * Generate the points for the current frame.
	 */
	std::vector<glm::vec3> GenerateFramePoints();

	/**
	 * Read frame data from image at filepath and increment current frame ID.
	 */
	void ReadFrameAndIncrement();

	/**
	 * Set the general filepath for reading frames. "_<frame number>.bmp" will be added to this path.
	 * \param filepath - The general filepath to the images. 
	 */
	void SetFilepath(const std::string& filepath);

private:
	unsigned char* ReadBMP(unsigned int frameID);

	unsigned int width;
	unsigned int height;
	std::string filepath;

	unsigned int currentFrameID;
	unsigned char* currentFrameData;
};