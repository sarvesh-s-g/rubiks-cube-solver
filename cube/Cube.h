#ifndef CUBE_H
#define CUBE_H
#include <string>

class Cube
{
private:
    // Cube has 6 faces,9 stickers each
    // Face indices: 0=UP, 1=LEFT, 2=FRONT, 3=RIGHT, 4=BACK, 5=DOWN
    char faces[6][9];
    void rotateFaceClockwise(int face);

public:
    Cube();
    void setFace(int face, const char colors[9]);
    void getFace(int face, char colors[9]) const;
    char getSticker(int face, int position) const;
    void print() const;
    bool isSolved() const;
    void moveU();  // Rotate UP face clockwise
    void moveD();  // Rotate DOWN face clockwise
    void moveL();  // Rotate LEFT face clockwise
    void moveR();  // Rotate RIGHT face clockwise
    void moveF();  // Rotate FRONT face clockwise
    void moveB();  // Rotate BACK face clockwise

    void moveUPrime();  // U' = U 3 times
    void moveDPrime();  // D'
    void moveLPrime();  // L'
    void moveRPrime();  // R'
    void moveFPrime();  // F'
    void moveBPrime();  // B'
    
    void moveU2();  // U2 = U twice
    void moveD2();  // D2
    void moveL2();  // L2
    void moveR2();  // R2
    void moveF2();  // F2
    void moveB2();  // B2
    
    void executeMove(const std::string& move);
    void move(int moveIndex);
};

// Face indices as constants (for readability)
enum Move {
    U = 0, U_PRIME, U2,
    D, D_PRIME, D2,
    L, L_PRIME, L2,
    R, R_PRIME, R2,
    F, F_PRIME, F2,
    B, B_PRIME, B2,
    NONE
};
const int FACE_UP    = 0;
const int FACE_LEFT  = 1;
const int FACE_FRONT = 2;
const int FACE_RIGHT = 3;
const int FACE_BACK  = 4;
const int FACE_DOWN  = 5;

#endif