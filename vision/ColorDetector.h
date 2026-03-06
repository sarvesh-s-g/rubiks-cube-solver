#ifndef COLOR_DETECTOR_H
#define COLOR_DETECTOR_H

// This function takes HSV values and returns the detected color
//'R'=Red, 'O'=Orange, 'Y'=Yellow, 'G'=Green, 'B'=Blue, 'W'=White, 'K'=Black

inline char detectColor(int H, int S, int V)
{
    if (V < 50)
    {
        return 'K';
    }
    if (S < 50 && V > 140)
    {
        return 'W';
    }
    if (H >= 5 && H <= 25 && S >= 40 && V >= 80)
    {
        return 'O';
    }
    if (((H >= 0 && H <= 8) || (H >= 165 && H <= 180)) && S >= 40 && V >= 80)
    {
        return 'R';
    }
    if (H >= 20 && H <= 40 && S >= 40 && V >= 90)
    {
        return 'Y';
    }
    if (H >= 40 && H <= 85 && S >= 30 && V >= 60)
    {
        return 'G';
    }
    if (H >= 85 && H <= 135 && S >= 50 && V >= 60)
    {
        return 'B';
    }

    return 'W';
}

#endif 