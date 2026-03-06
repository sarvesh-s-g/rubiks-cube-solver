#include "Camera.h"
#include <iostream>
using namespace std;

Camera::Camera()
{
    gridSize = 300;              
    cellSize = gridSize / 3;     
    startX = 0;
    startY = 0;
    cout << "[Camera] Camera object created\n";
}
Camera::~Camera()
{
    stop();
    cout << "[Camera] Camera object destroyed\n";
}

bool Camera::start()
{
    cap.open(0);  //0 means default camera
    if (!cap.isOpened())
    {
        cout << "[Camera] ERROR: Could not open camera!\n";
        return false;
    }
    cap.set(cv::CAP_PROP_AUTOFOCUS, 1);      // Enable autofocus
    cap.set(cv::CAP_PROP_AUTO_WB, 1);        // Enable auto white balance
    
    cout << "[Camera] Camera started successfully\n";
    return true;
}

void Camera::stop()
{
    if (cap.isOpened())
    {
        cap.release(); 
        cv::destroyAllWindows();
        cout << "[Camera] Camera stopped\n";
    }
}

bool Camera::captureFrame()
{
    cap >> frame;
    if (frame.empty())
    {
        cout << "[Camera] ERROR: Empty frame received\n";
        return false;
    }
    cv::GaussianBlur(frame, blurred, cv::Size(5, 5), 0);
    // BGR -> HSV
    cv::cvtColor(blurred, hsv, cv::COLOR_BGR2HSV);
    startX = frame.cols / 2 - gridSize / 2; 
    startY = frame.rows / 2 - gridSize / 2; 
    return true;
}

void Camera::drawGrid()
{
    //Horizontal lines
    for (int i = 0; i <= 3; i++)
    {
        cv::line(frame,
            cv::Point(startX, startY + i * cellSize),           // Start point
            cv::Point(startX + gridSize, startY + i * cellSize), // End point
            cv::Scalar(0, 255, 0),                               // Green color
            2);                                                   // 2 pixels thick
    }
    
    //Vertical lines
    for (int i = 0; i <= 3; i++)
    {
        cv::line(frame,
            cv::Point(startX + i * cellSize, startY),            // Start point
            cv::Point(startX + i * cellSize, startY + gridSize), // End point
            cv::Scalar(0, 255, 0),                                // Green color
            2);                                                    // 2 pixels thick
    }
}

void Camera::drawDetectedColors(const vector<char>& colors)
{
    int index = 0;
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            // finding centre
            int cx = startX + col * cellSize + cellSize / 2;
            int cy = startY + row * cellSize + cellSize / 2;
            //Sampling rectangle (30x30 pixels)
            int box = 30;
            cv::Rect roi(cx - box/2, cy - box/2, box, box);
            cv::rectangle(frame, roi, cv::Scalar(255, 255, 0), 1);  // Yellow rectangle
 
            char color = colors[index++];
            cv::Scalar circleColor;
            switch(color)
            {
                case 'R': circleColor = cv::Scalar(0, 0, 255);     break;  // Red
                case 'O': circleColor = cv::Scalar(0, 165, 255);   break;  // Orange
                case 'Y': circleColor = cv::Scalar(0, 255, 255);   break;  // Yellow
                case 'G': circleColor = cv::Scalar(0, 255, 0);     break;  // Green
                case 'B': circleColor = cv::Scalar(255, 0, 0);     break;  // Blue
                case 'W': circleColor = cv::Scalar(255, 255, 255); break;  // White
                default:  circleColor = cv::Scalar(128, 128, 128); break;  // Gray (unknown)
            }
            
            // Draw filled circle
            cv::circle(frame, cv::Point(cx, cy), 15, circleColor, -1); 
            // Draw color letter on top
            cv::putText(frame, 
                string(1, color),                      // Convert char to string
                cv::Point(cx - 8, cy + 8),             // Position (slightly offset)
                cv::FONT_HERSHEY_SIMPLEX,              // Font type
                0.6,                                    // Font scale
                cv::Scalar(0, 0, 0),                   // Black text
                2);                                     // Thickness
        }
    }
}

void Camera::showFrame()
{
    drawGrid();  // Draw the 3x3 grid
    cv::imshow("Rubik's Cube Scanner", frame);  // Show in window
}

vector<char> Camera::detectColors()
{
    vector<char> colors;
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            int cx = startX + col * cellSize + cellSize / 2;
            int cy = startY + row * cellSize + cellSize / 2;
            int box = 30;
            cv::Rect roi(cx - box/2, cy - box/2, box, box);
            cv::Scalar avg = cv::mean(hsv(roi));
            
            // Extract H, S, V values
            int H = (int)avg[0];  // Hue (0-180)
            int S = (int)avg[1];  // Saturation (0-255)
            int V = (int)avg[2];  // Value/Brightness (0-255)
            char detectedColor = detectColor(H, S, V);
            colors.push_back(detectedColor);
        }
    }
    drawDetectedColors(colors);
    return colors;
}

int Camera::waitKey(int delay)
{
    return cv::waitKey(delay);
}