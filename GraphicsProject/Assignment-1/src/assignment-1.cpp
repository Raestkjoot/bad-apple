#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ifile.h"
#include "glmutils.h"
#include "linerasterizer.h"
#include "badapple.h"
#include "shader_path.h"



/**
 * \file
 */

 /**
  * defines what should be drawn
  * \param GRID - if defined a grid is drawn.
  * \param TESTLINE - if defined the control line is drawn.
  * \param DOTLINE - if defined the approximated line is drawn as dots.
  */
typedef unsigned int uint;


/**
 * Global Variables
 * \param method - the method to be used to scan-convert a line.
 * \param xmin - the minimum x-coordinate.
 * \param xmax - the maximum x-coordinate.
 * \param ymin - the minimum y-coordinate.
 * \param ymax - the maximum y-coordinate.
 * \param xstart - the x-coordinate of the first point of the line
 * \param ystart - the y-coordinate of the first point of the line
 * \param xstop - the x-coordinate of the last point of the line
 * \param ystop - the y-coordinate of the last point of the line
 * \param WindowWidth - the current width of the window
 * \param WindowHeight - the current height of the window
 * \param WindowTitle - the current title of the window
 * \param NGridLines - the number of gridlines
 * \param PointSize - the pointsize
 * \param CoordinatesChaged - true if the arrow keya has been pressed,
 *                            false otherwize
 * \param NeedsUpdate - true if the window needs to be updated - keypress or window resize,
 *                      false otherwize
 */
// SETTINGS: grid
int method = 1;
int xmin = -24;
int xmax = 24;
int ymin = -18;
int ymax = 18;
int xstart = 0;
int ystart = 0;
int xstop = xmax;
int ystop = 0;

int WindowWidth = 500;
int WindowHeight = 500;
std::string WindowTitle("Bad Apple");

int   NGridLines = glm::max(xmax, ymax) * 2 + 3;
float PointSize = 1.0f;
float LineVertexScale = 1.0f / (glm::max(xmax, ymax) + 2.0f);

// SETTINGS: Bad Apple variables
BadApple badApple(48, 36, shader_path + "Frames/frame");
double fps = 6.2;

// runtime stuff
std::chrono::time_point<std::chrono::steady_clock> loopStartTime, loopEndTime;
std::chrono::duration<double, std::milli> timeSinceLastFrame;

bool CoordinatesChanged = false;
bool NeedsUpdate = true;

/**
 * Converts an OpenGL error code to a human readable text string
 * \param ErrorCode - the error code from the OpenGL system
 * \param Verbose - if false return a short error message,
 *                  else if true return a more verbose error message
 * \return - a human readable error message
 */
std::string ErrorMessage(GLenum ErrorCode, bool Verbose = false)
{
    Trace("", "ErrorMessage(GLenum, bool)");

    std::ostringstream errMessage;
    switch (ErrorCode) {
    case GL_NO_ERROR:
        errMessage << "no error";
        if (Verbose) {
            errMessage << "\nGlfwWindow: Error Code " << ErrorCode << ":" << std::endl
                << "\tNo error has been recorded.";
        }
        break;
    case GL_INVALID_ENUM:
        errMessage << "invalid enumerant";
        if (Verbose) {
            errMessage << "\nGlfwWindow: Error Code " << ErrorCode << ":" << std::endl
                << "\tAn unacceptable value is specified for an enumerated argument.";
            errMessage << "\n\tThe offending command is ignored and has no "
                << "other side effect than to set the error flag.";
        }
        break;
    case GL_INVALID_VALUE:
        errMessage << "invalid value";
        if (Verbose) {
            errMessage << "\nGlfwWindow: Error Code " << ErrorCode << ":" << std::endl
                << "\tA numeric argument is out of range.";
            errMessage << "\n\tThe offending command is ignored and has no "
                << "other side effect than to set the error flag.";
        }
        break;
    case GL_INVALID_OPERATION:
        errMessage << "invalid operation";
        if (Verbose) {
            errMessage << "\nGlfwWindow: Error Code " << ErrorCode << ":" << std::endl
                << "\tThe specified operation is not allowed in the current state.";
            errMessage << "\n\tThe offending command is ignored and has no "
                << "other side effect than to set the error flag.";
        }
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        errMessage << "invalid framebuffer operation";
        if (Verbose) {
            errMessage << "\nGlfwWindow: Error Code " << ErrorCode << ":" << std::endl
                << "\tThe framebuffer object is not complete.";
            errMessage << "\n\tThe offending command is ignored and has no "
                << "other side effect than to set the error flag.";
        }
        break;
    case GL_OUT_OF_MEMORY:
        errMessage << "out of memory";
        if (Verbose) {
            errMessage << "\nGlfwWindow: Error Code " << ErrorCode << ":" << std::endl
                << "\tThere is not enough memory left to execute the command.";
            errMessage << "\n\tThe state of the GL is undefined, "
                << "except for the state of the error flags, after this error is recorded.";
        }
        break;
    case GL_STACK_UNDERFLOW:
        errMessage << "stack underflow";
        if (Verbose) {
            errMessage << "\nGlfwWindow: Error Code " << ErrorCode << ":" << std::endl
                << "\tAn attempt has been made to perform an operation that "
                << "would cause an internal stack to underflow.";
        }
        break;
    case GL_STACK_OVERFLOW:
        errMessage << "stack overflow";
        if (Verbose) {
            errMessage << "\nGlfwWindow: Error Code " << ErrorCode << ":" << std::endl
                << "\tAn attempt has been made to perform an operation that "
                << "would cause an internal stack to overflow.";
        }
        break;
    default:
        errMessage << "GlfwWindow: Error Code " << ErrorCode << ": Unknown Error Code";
    }
    return errMessage.str();
}


/**
 * Clears any OpenGL errors.
 * \param Message - a string which should be written if any errors messages.
 * \param BeQuiet - if true no message is output, else an error message is output.
 */
void ErrorClear(std::string Message = "", bool BeQuiet = true)
{
    //std::cout << "-->ErrorClear()" << std::endl << std::flush;

    bool AnyErrors = false;
    GLenum ErrorCode = GL_NO_ERROR;
    std::ostringstream errormessage;
    errormessage << "ErrorClear(): " << Message << ": ";
    while ((ErrorCode = glGetError()) != GL_NO_ERROR) {
        AnyErrors = true;
        errormessage << ErrorMessage(ErrorCode);
    }
    if (AnyErrors) {
        if (!BeQuiet) {
            std::cerr << errormessage.str() << ": cleared" << std::endl;
        }
    }

    //std::cout << "<--ErrorClear()" << std::endl << std::flush;
}

/**
 * Checks if any errors occurred during processing of OpenGL requests
 * If an error has occurred an exception is thown.
 * \param Message - a message which is output before the actual error message.
 */
void ErrorCheck(std::string Message = "")
{
    // std::cout << "-->ErrorCheck()" << std::endl << std::flush;

    bool AnyErrors = false;
    GLenum ErrorCode = GL_NO_ERROR;
    std::ostringstream errormessage;
    errormessage << "ErrorCheck(): " << Message << ": ";
    while ((ErrorCode = glGetError()) != GL_NO_ERROR) {
        AnyErrors = true;
        errormessage << ErrorMessage(ErrorCode);
    }
    if (AnyErrors) {
        throw std::runtime_error(errormessage.str());
    }

    // std::cout << "<--ErrorCheck()" << std::endl << std::flush;
}

/**
 * Prints out a std::vector<glm::vec3> to an output stream
 * \param s - The output stream that should be printed to
 * \param data - The data that should be printed
 * \return the output stream s
 */
std::ostream& operator<<(std::ostream& s, std::vector<glm::vec3> const& data)
{
    for (int i = 0; i < data.size(); ++i) {
        s << data[i] << std::endl;
    }
    return s;
}

/**
 * Reads the contents of a text file and returns in as a string.
 * \param FileName - A string containing the name of the file to be read.
 * \return A sting containing the contents of the file.
 */
std::string Read(std::string const& FileName)
{
    std::string resultprog;
    InputFile Data(FileName);
    while (!Data.eof()) {
        std::string line;
        std::getline(Data, line);
        if (line.size() > 0) {
            resultprog += line + '\n';
        }
    }
    if (resultprog.size() <= 0) {
        std::ostringstream errormessage;
        errormessage << "No program found in file: " << FileName << std::endl;
        throw std::runtime_error(errormessage.str().c_str());
    }
    return resultprog;
}

/**
 * Generates quadratic grid
 */
std::vector<glm::vec3> GenerateGridLines()
{
    std::vector<glm::vec3> lines;

    for (float i = xmin; i <= xmax; i += 1.0f) {
        lines.push_back(glm::vec3(i, ymin, -0.05f));
        lines.push_back(glm::vec3(i, ymax, -0.05f));
    }
    for (float i = ymin; i <= ymax; i += 1.0f) {
        lines.push_back(glm::vec3(xmin, i, -0.05f));
        lines.push_back(glm::vec3(xmax, i, -0.05f));
    }
    return lines;
}

/**
 * Scanconverts a straight line, i.e. computes the pixels that represents the approximated straight line.
 * \param x1 - the x-coordinate of the start point.
 * \param y1 - the y-coordinate of the start point.
 * \param x2 - the x-coordinate of the end point.
 * \param y2 - the y-coordinate of the end point.
 * \return A std::vector which contains the coordinates of the pixels of a straight line.
 */
std::vector<glm::vec3> GenerateFramePixels()
{
    std::vector<glm::vec3> pixels;
    badApple.ReadFrameAndIncrement();
    pixels = badApple.GenerateFramePoints();

    CoordinatesChanged = true;
    NeedsUpdate = true;

    return pixels;
}


/**
 * Callback function for window resize
 * \param Window - A pointer to the window beeing resized
 * \param width - The new width of the window
 * \param height - The new height of the window
 */
void ResizeCallback(GLFWwindow* Window, int width, int height)
{
    // std::cout << "-->ResizeCallback(GLFWwindow* Window, int width, int height)" << std::endl;
    // std::cout << "   width = " << width << ", height = " << height << std::endl; 

    // Set the point size - make the size of the dot be a little smaller than the minimum distance
    // between the grid lines
    WindowWidth = width;
    WindowHeight = height;
    float dist = std::min(WindowWidth, WindowHeight);
    float pointsize = dist / NGridLines;
    PointSize = pointsize;
    int fwidth, fheight;
    glfwGetFramebufferSize(Window, &fwidth, &fheight);
    glViewport(0, 0, fwidth, fheight);
    NeedsUpdate = true;

    // std::cout << "<--ResizeCallback(GLFWwindow* Window, int width, int height)" << std::endl;
}

/**
 * Callback function for keyboard shortcuts
 * \param Window - the window that created the callback
 * \param key - The key code
 * \param scancode - Don't know right now? It is badly documented in GLFW
 * \param action - Can be any of GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
 * \param mods - Modifier keys pressed at the same time, like shift etc.
 */
void KeyboardCallback(GLFWwindow* Window, int key, int scancode, int action, int mode)
{
    if (action == GLFW_RELEASE) return;

    if (action == GLFW_PRESS) {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(Window, GL_TRUE);
            CoordinatesChanged = false;
            break;
        case GLFW_KEY_ENTER:
            badApple.SetCurrentFrame(1u);
            break;
        }

        CoordinatesChanged = true;
        NeedsUpdate = true;
    }
}

int main()
{
    try {
        // GLenum Error = GL_NO_ERROR;
 #pragma region Initialization


        // Initialize glfw
        if (!glfwInit()) {
            throw std::runtime_error("Glfw Failed to initialize");
        }

        // Set up some hints for the window
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create the actual window
        GLFWwindow* Window;
        Window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle.c_str(), NULL, NULL);
        if (!Window) {
            throw std::runtime_error("Could not create window");
        }
        if (Window != NULL) {
            glfwMakeContextCurrent(Window);
            int XwindowPos, YwindowPos;
            glfwGetWindowPos(Window, &XwindowPos, &YwindowPos);
            glfwSetWindowPos(Window, XwindowPos + 1, YwindowPos);
            glfwPollEvents();
        }

        // Setup a callback function for resize of the window
        glfwSetWindowSizeCallback(Window, ResizeCallback);

        // Setup a callback function for keyboard input - pres a key
        glfwSetKeyCallback(Window, KeyboardCallback);

        // Initialize glew
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            std::ostringstream errmess;
            errmess << "GlfwWindow::Initialize(): "
                << "GLEW failed to initialize: "
                << glewGetErrorString(err) << ", (" << err << ") \n"
                << "Status, Using GLEW version: " << glewGetString(GLEW_VERSION)
                << std::endl;
            throw std::runtime_error(errmess.str());
            std::cout << errmess.str();
        }

        // Clear any system errors if any
        ErrorClear("Right after glewInit()", false);

        // Initialize OpenGL
        glfwMakeContextCurrent(Window);
        glClearColor(0.5, 0.5, 0.5, 1.0);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL); // You might change this when the transformations are in place!
        glDrawBuffer(GL_BACK);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(Window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ErrorCheck("OpenGL Status:");

        // Read and Compile the vertex program vertexscale.vert
        std::string vertexprogram = Read(shader_path + "vertexscale.vert");
        GLuint vertexprogID = glCreateShader(GL_VERTEX_SHADER);
        if (vertexprogID == 0) {
            std::ostringstream errormessage;
            errormessage << "The vertex program could not be created" << std::endl;
            throw std::runtime_error(errormessage.str().c_str());
        }

        GLchar const* vertexprogramsrc[1];
        vertexprogramsrc[0] = vertexprogram.c_str();
        GLint vertexlengths[1];
        vertexlengths[0] = vertexprogram.size();

        glShaderSource(vertexprogID, 1, vertexprogramsrc, vertexlengths);
        glCompileShader(vertexprogID);

        GLint vertexsuccess = 0;
        glGetShaderiv(vertexprogID, GL_COMPILE_STATUS, &vertexsuccess);

        if (!vertexsuccess) {
            GLchar InfoLog[1024] = { 0 };
            glGetShaderInfoLog(vertexprogID, sizeof(InfoLog), NULL, InfoLog);

            std::ostringstream errormessage;
            errormessage << "Error compiling vertex program in file: vertexscale.vert: " << InfoLog << std::endl;
            throw std::runtime_error(errormessage.str().c_str());
        }

        // Read and Compile the fragment program linefragment.frag
        std::string linefragmentprogram = Read(shader_path + "linefragment.frag");
        GLuint linefragmentprogID = glCreateShader(GL_FRAGMENT_SHADER);
        if (linefragmentprogID == 0) {
            std::ostringstream errormessage;
            errormessage << "The fragment program could not be created" << std::endl;
            throw std::runtime_error(errormessage.str().c_str());
        }

        GLchar const* linefragmentprogramsrc[1];
        linefragmentprogramsrc[0] = linefragmentprogram.c_str();
        GLint linefragmentlengths[1];
        linefragmentlengths[0] = linefragmentprogram.size();

        glShaderSource(linefragmentprogID, 1, linefragmentprogramsrc, linefragmentlengths);
        glCompileShader(linefragmentprogID);

        GLint linefragmentsuccess = 0;
        glGetShaderiv(linefragmentprogID, GL_COMPILE_STATUS, &linefragmentsuccess);

        if (!linefragmentsuccess) {
            GLchar InfoLog[1024] = { 0 };
            glGetShaderInfoLog(linefragmentprogID, sizeof(InfoLog), NULL, InfoLog);

            std::ostringstream errormessage;
            errormessage << "Error compiling fragment program in file: linefragment.frag: " << InfoLog << std::endl;
            throw std::runtime_error(errormessage.str().c_str());
        }

        // Create a lineshader program and Link it with the vertex and linefragment programs
        GLuint lineshaderID = glCreateProgram();
        if (lineshaderID == 0) {
            std::ostringstream errormessage;
            errormessage << "Could not create shaderprogram" << std::endl;
            throw std::runtime_error(errormessage.str());
        }
        glAttachShader(lineshaderID, vertexprogID);
        glAttachShader(lineshaderID, linefragmentprogID);

        GLint lineprogramsuccess = 0;
        glLinkProgram(lineshaderID);
        glGetProgramiv(lineshaderID, GL_LINK_STATUS, &lineprogramsuccess);
        if (!lineprogramsuccess) {
            GLchar InfoLog[1024] = { 0 };
            glGetProgramInfoLog(lineshaderID, sizeof(InfoLog), NULL, InfoLog);

            std::ostringstream errormessage;
            errormessage << "Error linking shaderprogram: " << InfoLog << std::endl;
            throw std::runtime_error(errormessage.str().c_str());
        }
        glDetachShader(lineshaderID, vertexprogID);
        glDetachShader(lineshaderID, linefragmentprogID);

        // Read and Compile the fragment program dotfragment.frag
        std::string dotfragmentprogram = Read(shader_path + "dotfragment.frag");
        GLuint dotfragmentprogID = glCreateShader(GL_FRAGMENT_SHADER);
        if (dotfragmentprogID == 0) {
            std::ostringstream errormessage;
            errormessage << "The fragment program could not be created" << std::endl;
            throw std::runtime_error(errormessage.str().c_str());
        }

        GLchar const* dotfragmentprogramsrc[1];
        dotfragmentprogramsrc[0] = dotfragmentprogram.c_str();
        GLint fragmentlengths[1];
        fragmentlengths[0] = dotfragmentprogram.size();

        glShaderSource(dotfragmentprogID, 1, dotfragmentprogramsrc, fragmentlengths);
        glCompileShader(dotfragmentprogID);

        GLint dotfragmentsuccess = 0;
        glGetShaderiv(dotfragmentprogID, GL_COMPILE_STATUS, &dotfragmentsuccess);

        if (!dotfragmentsuccess) {
            GLchar InfoLog[1024] = { 0 };
            glGetShaderInfoLog(dotfragmentprogID, sizeof(InfoLog), NULL, InfoLog);

            std::ostringstream errormessage;
            errormessage << "Error compiling fragment program in file: dotfragment.frag: " << InfoLog << std::endl;
            throw std::runtime_error(errormessage.str().c_str());
        }

        // Create dotshader program and Link it with the vertex and dotfragment programs
        GLuint dotshaderID = glCreateProgram();
        if (dotshaderID == 0) {
            std::ostringstream errormessage;
            errormessage << "Could not create shaderprogram" << std::endl;
            throw std::runtime_error(errormessage.str());
        }
        glAttachShader(dotshaderID, vertexprogID);
        glAttachShader(dotshaderID, dotfragmentprogID);

        GLint dotprogramsuccess = 0;
        glLinkProgram(dotshaderID);
        glGetProgramiv(dotshaderID, GL_LINK_STATUS, &dotprogramsuccess);
        if (!dotprogramsuccess) {
            GLchar InfoLog[1024] = { 0 };
            glGetProgramInfoLog(dotshaderID, sizeof(InfoLog), NULL, InfoLog);

            std::ostringstream errormessage;
            errormessage << "Error linking shaderprogram: " << InfoLog << std::endl;
            throw std::runtime_error(errormessage.str().c_str());
        }
        glDetachShader(dotshaderID, vertexprogID);
        glDetachShader(dotshaderID, dotfragmentprogID);

        // Now comes the OpenGL core part

        // This is where the grid is initialized

        // User data
        std::vector<glm::vec3> GridLines;
        GridLines = GenerateGridLines();

        // Make a VertexArrayObject - it is used by the VertexArrayBuffer, and it must be declared!
        GLuint GridVertexArrayID;
        glGenVertexArrays(1, &GridVertexArrayID);
        glBindVertexArray(GridVertexArrayID);

        // Make a GridvertexbufferObject - it uses the previous VertexArrayBuffer!
        GLuint gridvertexbuffer;
        glGenBuffers(1, &gridvertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, gridvertexbuffer);

        // Give our vertices to OpenGL.
        if (GridLines.size() > 0) {
            glBufferData(GL_ARRAY_BUFFER, GridLines.size() * sizeof(float) * 3, &(GridLines[0][0]), GL_STATIC_DRAW);
        }

        // Validate the grid shader program
        GLint gridvalidationsuccess = 0;
        glValidateProgram(lineshaderID);
        glGetProgramiv(lineshaderID, GL_VALIDATE_STATUS, &gridvalidationsuccess);
        if (!gridvalidationsuccess) {
            GLchar InfoLog[1024] = { 0 };
            glGetProgramInfoLog(lineshaderID, sizeof(InfoLog), NULL, InfoLog);

            std::ostringstream errormessage;
            errormessage << "Error validating the line shader program failed: " << InfoLog << std::endl;
            throw std::runtime_error(errormessage.str().c_str());
        }

        // Get locations of Uniforms
        GLuint linevertexscale = glGetUniformLocation(lineshaderID, "Scale");
        // GLuint linevertexpointsize = glGetUniformLocation(lineshaderID, "PointSize");
        GLuint linefragmentcolor = glGetUniformLocation(lineshaderID, "Color");


        // Initialize grid Attributes
        GLuint linearvertexattribute = glGetAttribLocation(lineshaderID, "VertexPosition");
        glVertexAttribPointer(linearvertexattribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Unbind the vertex array
        glBindVertexArray(0);

        // This where the dots of the lines initialized

        // User data
        std::vector<glm::vec3> FramePixels;
        FramePixels = GenerateFramePixels();
        //std::cout << LinePixels << std::endl;

        // Make a VertexArrayObject - it is used by the VertexArrayBuffer, and it must be declared!
        GLuint PixelVertexArrayID;
        glGenVertexArrays(1, &PixelVertexArrayID);
        glBindVertexArray(PixelVertexArrayID);

        // Make a VertexBufferObject - it uses the previous VertexArrayBuffer!
        GLuint dotvertexbuffer;
        glGenBuffers(1, &dotvertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, dotvertexbuffer);

        // Give our vertices to OpenGL.
        if (FramePixels.size() > 0) {
            glBufferData(GL_ARRAY_BUFFER, FramePixels.size() * sizeof(float) * 3, &(FramePixels[0][0]), GL_STATIC_DRAW);
        }

        // Validate the dot shader program
        GLint dotvalidationsuccess = 0;
        glValidateProgram(dotshaderID);
        glGetProgramiv(dotshaderID, GL_VALIDATE_STATUS, &dotvalidationsuccess);
        if (!dotvalidationsuccess) {
            GLchar InfoLog[1024] = { 0 };
            glGetProgramInfoLog(dotshaderID, sizeof(InfoLog), NULL, InfoLog);

            std::ostringstream errormessage;
            errormessage << "Error validating the dot shader program failed: " << InfoLog << std::endl;
            throw std::runtime_error(errormessage.str().c_str());
        }

        // Get locations of Dot Uniforms
        GLuint dotvertexscale = glGetUniformLocation(dotshaderID, "Scale");
        GLuint dotvertexpointsize = glGetUniformLocation(dotshaderID, "PointSize");
        GLuint dotfragmentcolor = glGetUniformLocation(dotshaderID, "Color");

        // Initialize dot Attributes
        GLuint dotvertexattribute = glGetAttribLocation(dotshaderID, "VertexPosition");
        glVertexAttribPointer(dotvertexattribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Unbind the vertex array
        glBindVertexArray(0);


        // Set the point size - make the size of the dot be a little smaller than the minimum distance
        // between the grid lines
        glEnable(GL_PROGRAM_POINT_SIZE);
        float dist = std::min(WindowWidth, WindowHeight);
        float pointsize = dist / NGridLines;
        PointSize = pointsize;
        // std::cout << "PointSize = " << PointSize << std::endl;

        // The main loop
        std::cout << std::endl;
        std::cout << "**********************************************************************" << std::endl;
        std::cout << "* Bad Apple Music Video in DIKU's Graphics Programming Framework     *" << std::endl;
        std::cout << "*                                                                    *" << std::endl;
        std::cout << "* Press ENTER to reset                                               *" << std::endl;
        std::cout << "* Press ESC to finish the program                                    *" << std::endl;
        std::cout << "**********************************************************************" << std::endl;
        std::cout << std::endl;
        #pragma endregion

        while (!glfwWindowShouldClose(Window)) {
            try {
                loopStartTime = std::chrono::steady_clock::now();
                if (NeedsUpdate) {
                    glfwMakeContextCurrent(Window);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    // Generate grid
                    glUseProgram(lineshaderID);
                    glUniform1f(linevertexscale, LineVertexScale);
                    glUniform3f(linefragmentcolor, 0.0f, 0.0f, 1.0f);

                    glBindVertexArray(GridVertexArrayID);
                    glEnableVertexAttribArray(linearvertexattribute);
                    if (GridLines.size() > 0) {
                        glDrawArrays(GL_LINES, 0, GridLines.size());
                    }
                    glDisableVertexAttribArray(linearvertexattribute);
                    glUseProgram(0);

                    // Generate dots
                    glUseProgram(dotshaderID);
                    glUniform1f(dotvertexscale, LineVertexScale);
                    glUniform1f(dotvertexpointsize, PointSize);
                    glUniform3f(dotfragmentcolor, 0.0f, 0.0f, 0.0f);

                    glBindVertexArray(PixelVertexArrayID);
                    glEnableVertexAttribArray(dotvertexattribute);
                    if (CoordinatesChanged) {
                        FramePixels = GenerateFramePixels();
                        glBindBuffer(GL_ARRAY_BUFFER, dotvertexbuffer);
                        if (FramePixels.size() > 0) {
                            glBufferData(GL_ARRAY_BUFFER, FramePixels.size() * sizeof(float) * 3, &(FramePixels[0][0]),
                                GL_STATIC_DRAW);
                        }
                    }
                    if (FramePixels.size() > 0) {
                        glDrawArrays(GL_POINTS, 0, FramePixels.size());
                    }
                    glDisableVertexAttribArray(dotvertexattribute);
                    glUseProgram(0);

                    // Render frame
                    glfwSwapBuffers(Window);

                    CoordinatesChanged = false;
                    NeedsUpdate = false;
                }
                glfwPollEvents();

                loopEndTime = std::chrono::steady_clock::now();
                timeSinceLastFrame += loopEndTime - loopStartTime;
                if (timeSinceLastFrame.count() >= 1000.0 / (fps)) {
                    CoordinatesChanged = true;
                    NeedsUpdate = true;
                    timeSinceLastFrame = std::chrono::duration<double, std::milli>(0.0);
                }
            }
            catch (std::exception& Exception) {
                std::cerr << Exception.what() << std::endl;
            }
        }
    }
    catch (std::exception const& runtimeerror) {
        std::cerr << "Exception: " << runtimeerror.what() << std::endl;
    }

    glfwTerminate();

    return 0;
}
