#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <sstream>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1;
#include <experimental/filesystem>

using namespace cv;

int main(int argc, char** argv) {
    // Open video file
    VideoCapture cap("Data/BadApple.mp4");
    if (!cap.isOpened())
    {
        std::cout << "Cannot open the video file" << std::endl;
        return -1;
    }

    // Get the frames-per-second of the video
    double fps = cap.get(CAP_PROP_FPS);
    std::cout << "Frame per seconds : " << fps << std::endl;
    // Create a window called "MyVideo"
    namedWindow("MyVideo", WINDOW_AUTOSIZE);
    // Create a 'Frames' folder we can save the images in
    std::experimental::filesystem::path root = std::experimental::filesystem::current_path();
    std::experimental::filesystem::create_directories(root / "Frames");

    // c is the frame count. We will save every n-th frame. 
    int c = 0;
    int n = 5;
    while (true)
    {
        // attempt to read a frame from the video
        Mat frame;
        if (!cap.read(frame))
        {
            std::cout << "Cannot read the frame from video file" << std::endl;
            break;
        }

        c++;
        // Save every n-th frame
        if (c % n == 0)
        {
            // show the frame in a window
            imshow("MyVideo", frame);

            // convert frame to a smaller binary grayscale bitmap
            Mat smaller, gray, binary;
            resize(frame, smaller, Size(), 0.1, 0.1, INTER_NEAREST);
            smaller.convertTo(gray, CV_8UC1);
            threshold(gray, binary, 175, UINT8_MAX, THRESH_BINARY);

            // save image file in the Frames folder
            imwrite("Frames/frame_" + std::to_string(c / n) + ".bmp", binary);

            // exit if esc key is pressed
            if (waitKey(30) == 27)
            {
                std::cout << "esc key is pressed by user" << std::endl;
                break;
            }
        }
    }

    return 0;
}