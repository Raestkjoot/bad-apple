#include "linerasterizer.h"


/*
 * \class LineRasterizer
 * A class which scanconverts a straight line. It computes the pixels such that they are as close to the
 * the ideal line as possible.
 */ 

/*
 * Parameterized constructor creates an instance of a line rasterizer
 * \param x1 - the x-coordinate of the first vertex
 * \param y1 - the y-coordinate of the first vertex
 * \param x2 - the x-coordinate of the second vertex
 * \param y2 - the y-coordinate of the second vertex
 */
LineRasterizer::LineRasterizer(int x1, int y1, int x2, int y2)
{
    this->initialize_line(x1, y1, x2, y2);
}

/*
 * Destroys the current instance of the line rasterizer
 */
LineRasterizer::~LineRasterizer()
{}

/*
 * Initializes the LineRasterizer with a new line
 * \param x1 - The x-coordinate of the first vertex
 * \param y1 - The y-coorsinate of the first vertex
 * \param x2 - The x-coordinate of the second vertex
 * \param y2 - The y-coordinate of the second vertex
 */
void LineRasterizer::Init(int x1, int y1, int x2, int y2)
{
    this->initialize_line(x1, y1, x2, y2);
}

/*
 * Checks if there are fragments/pixels of the line ready for use
 * \return true if there are more fragments of the line, else false is returned
 */
bool LineRasterizer::MoreFragments() const
{
    return this->valid;
}

/*
 * Computes the next fragment of the line
 */
void LineRasterizer::NextFragment()
{
    // Run the innerloop once
    // Dereference the pointer to the private member function 
    // It looks strange; but this is the way it is done!
    (this->*innerloop)();
}

/*
 * Returns a vector which contains all the pixels of the line
 */
std::vector<glm::vec3> LineRasterizer::AllFragments()
{
    std::vector<glm::vec3> points;

    while (LineRasterizer::MoreFragments())
    {
        int x = LineRasterizer::x();
        int y = LineRasterizer::y();

        glm::vec3 point(x, y, 0.0f);
        points.push_back(point);

        LineRasterizer::NextFragment();
    }
    return points;
}

/*
 * Returns the coordinates of the current fragment/pixel of the line.
 * It is only valid to call this function if "MoreFragments()" returns true,
 * else a "runtime_error" exception is thrown
 * \return The coordinates of the current line fragment/pixel
 */
glm::vec3 LineRasterizer::Fragment() const
{
    return glm::vec3(float(this->x()), float(this->y()), 0.0f);
}

/*
 * Returns the current x-coordinate of the current fragment/pixel of the line
 * It is only valid to call this function if "more_fragments()" returns true,
 * else a "runtime_error" exception is thrown
 * \return The x-coordinate of the current line fragment/pixel
 */
int LineRasterizer::x() const
{
    if (!this->valid) {
        throw std::runtime_error("LineRasterizer::x(): Invalid State");
    }
    return this->x_current;
}

/*
 * Returns the current y-coordinate of the current fragment/pixel of the line
 * It is only valid to call this function if "more_fragments()" returns true,
 * else a "runtime_error" exception is thrown
 * \return The y-coordinate of the current line fragment/pixel
 */
int LineRasterizer::y() const
{
    if (!this->valid) {
        throw std::runtime_error("LineRasterizer::y(): Invalid State");
    }
    return this->y_current;
}

/*
 * Protected functions
 */

/*
 * Private functions
 */

/*
 * Initializes the LineRasterizer with the two vertices
 */
void LineRasterizer::initialize_line(int x1, int y1, int x2, int y2)
{
    this->x_start = x1;
    this->y_start = y1;

    this->x_stop  = x2;
    this->y_stop  = y2;

    this->x_current = this->x_start;
    this->y_current = this->y_start;

    this->dx = this->x_stop - this->x_start;
    this->dy = this->y_stop - this->y_start;

    this->abs_2dx = std::abs(this->dx) << 1; // 2 * |dx|
    this->abs_2dy = std::abs(this->dy) << 1; // 2 * |dy|

    this->x_step = (dx < 0) ? -1 : 1;
    this->y_step = (dy < 0) ? -1 : 1;

    bool x_dominant = (this->abs_2dx > this->abs_2dy);

    if (x_dominant)
    {
        // init x dominant loop
        this->left_right = (this->x_step > 0);
        this->d = this->abs_2dy - (this->abs_2dx >> 1); // 2 * |dy| - |dx|
        this->valid = (this->x_start != this->x_stop);

        this->innerloop = &LineRasterizer::x_dominant_innerloop;
    }
    else
    {
        // init y dominant loop
        this->left_right = (this->y_step > 0);
        this->d = this->abs_2dx - (this->abs_2dy >> 1); // 2 * |dx| - |dy|
        this->valid = (this->y_start != this->y_stop);

        this->innerloop = &LineRasterizer::y_dominant_innerloop;
    }
}

/*
 * Runs the x-dominant innerloop
 */
void LineRasterizer::x_dominant_innerloop()
{
    if (this->x_current == this->x_stop) 
        this->valid = false;
    else
    {
        if ((this->d > 0) || 
            ((this->d == 0) && this->left_right))
        {
            this->y_current += this->y_step;
            this->d -= this->abs_2dx;
        }
        this->x_current += this->x_step;
        this->d += this->abs_2dy;
    }
}

/*
 * Runs the y-dominant innerloop
 */
void LineRasterizer::y_dominant_innerloop()
{    
    if (this->y_current == this->y_stop) 
        this->valid = false;
    else 
    {
        if ((this->d > 0) ||
            ((this->d == 0) && this->left_right))
        {
            this->x_current += this->x_step;
            this->d -= this->abs_2dy;
        }
        this->y_current += this->y_step;
        this->d += this->abs_2dx;
    } 
}