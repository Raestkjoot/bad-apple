#pragma once

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <vector>

#include "traceinfo.h"
#include "glmutils.h"


/**
 * \class BadApple
 * A class which reads a frame from an image and generates vec3 points that can be drawn with OpenGL
 */
class BadApple {
public:
	std::vector<glm::vec3> GenerateFramePoints();
};