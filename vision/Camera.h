#ifndef CAMERA_H
#define CAMERA_H
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include "ColorDetector.h"
using namespace std;
class Camera
{
private:
    cv::VideoCapture cap;        // OpenCV camera object
    cv::Mat frame;               // Current camera frame (BGR format)
    cv::Mat hsv;                 // Same frame in HSV format
    cv::Mat blurred;             // Blurred frame
    
    int gridSize;                
    int cellSize;                
    int startX, startY;          
    void drawGrid();
    void drawDetectedColors(const vector<char>& colors);

public:
    Camera();
    ~Camera();
    bool start();
    void stop();
    bool captureFrame();
    void showFrame();
    vector<char> detectColors();
    int waitKey(int delay = 1);
};

#endif