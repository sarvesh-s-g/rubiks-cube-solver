#include "Cube.h"
#include <iostream>
#include <iomanip>
using namespace std;

Cube::Cube()
{
    for (int i = 0; i < 9; i++) faces[FACE_UP][i]    = 'W';
    for (int i = 0; i < 9; i++) faces[FACE_LEFT][i]  = 'R';
    for (int i = 0; i < 9; i++) faces[FACE_FRONT][i] = 'G';
    for (int i = 0; i < 9; i++) faces[FACE_RIGHT][i] = 'O';
    for (int i = 0; i < 9; i++) faces[FACE_BACK][i]  = 'B';
    for (int i = 0; i < 9; i++) faces[FACE_DOWN][i]  = 'Y';
}
void Cube::setFace(int face, const char colors[9])
{
    if (face < 0 || face > 5) return;
    for (int i = 0; i < 9; i++)
        faces[face][i] = colors[i];
}
void Cube::getFace(int face, char colors[9]) const
{
    if (face < 0 || face > 5) return;
    for (int i = 0; i < 9; i++)
        colors[i] = faces[face][i];
}
char Cube::getSticker(int face, int position) const
{
    if (face < 0 || face > 5 || position < 0 || position > 8)
        return '?';
    return faces[face][position];
}
void Cube::rotateFaceClockwise(int face)
{
    char temp[9];
    for (int i = 0; i < 9; i++)
        temp[i] = faces[face][i];
    //If u want to understand this then draw a cube and see
    faces[face][0] = temp[6];
    faces[face][1] = temp[3];
    faces[face][2] = temp[0];
    faces[face][3] = temp[7];
    faces[face][4] = temp[4];
    faces[face][5] = temp[1];
    faces[face][6] = temp[8];
    faces[face][7] = temp[5];
    faces[face][8] = temp[2];
}

void Cube::print() const
{
    cout << "\nCube State:\n";
    cout << "===========\n\n";
    cout << "      UP\n";
    for (int row = 0; row < 3; row++)
    {
        cout << "      ";
        for (int col = 0; col < 3; col++)
            cout << faces[FACE_UP][row * 3 + col] << " ";
        cout << "\n";
    }
    cout << "\n";
    cout << "LEFT  FRONT RIGHT BACK\n";
    for (int row = 0; row < 3; row++)
    {
        // LEFT
        for (int col = 0; col < 3; col++)
            cout << faces[FACE_LEFT][row * 3 + col] << " ";
        cout << " ";
        
        // FRONT
        for (int col = 0; col < 3; col++)
            cout << faces[FACE_FRONT][row * 3 + col] << " ";
        cout << " ";
        
        // RIGHT
        for (int col = 0; col < 3; col++)
            cout << faces[FACE_RIGHT][row * 3 + col] << " ";
        cout << " ";
        
        // BACK
        for (int col = 0; col < 3; col++)
            cout << faces[FACE_BACK][row * 3 + col] << " ";
        cout << "\n";
    }
    cout << "\n";
    cout << "      DOWN\n";
    for (int row = 0; row < 3; row++)
    {
        cout << "      ";
        for (int col = 0; col < 3; col++)
            cout << faces[FACE_DOWN][row * 3 + col] << " ";
        cout << "\n";
    }
    cout << "\n===========\n";
}
bool Cube::isSolved() const
{
    for (int face = 0; face < 6; face++)
    {
        char centerColor = faces[face][4];
        for (int pos = 0; pos < 9; pos++)
        {
            if (faces[face][pos] != centerColor)
                return false;
        }
    }
    return true;
}

void Cube::moveU()
{
    rotateFaceClockwise(FACE_UP);
    char temp[3] = {
        faces[FACE_FRONT][0],
        faces[FACE_FRONT][1],
        faces[FACE_FRONT][2]
    };
    
    // Cycle: FRONT ← RIGHT ← BACK ← LEFT ← FRONT
    faces[FACE_FRONT][0] = faces[FACE_RIGHT][0];
    faces[FACE_FRONT][1] = faces[FACE_RIGHT][1];
    faces[FACE_FRONT][2] = faces[FACE_RIGHT][2];
    
    faces[FACE_RIGHT][0] = faces[FACE_BACK][0];
    faces[FACE_RIGHT][1] = faces[FACE_BACK][1];
    faces[FACE_RIGHT][2] = faces[FACE_BACK][2];
    
    faces[FACE_BACK][0] = faces[FACE_LEFT][0];
    faces[FACE_BACK][1] = faces[FACE_LEFT][1];
    faces[FACE_BACK][2] = faces[FACE_LEFT][2];
    
    faces[FACE_LEFT][0] = temp[0];
    faces[FACE_LEFT][1] = temp[1];
    faces[FACE_LEFT][2] = temp[2];
}
void Cube::moveD()
{
    rotateFaceClockwise(FACE_DOWN);
    char temp[3] = {
        faces[FACE_FRONT][6],
        faces[FACE_FRONT][7],
        faces[FACE_FRONT][8]
    };
    
    // Cycle: FRONT ← LEFT ← BACK ← RIGHT ← FRONT
    faces[FACE_FRONT][6] = faces[FACE_LEFT][6];
    faces[FACE_FRONT][7] = faces[FACE_LEFT][7];
    faces[FACE_FRONT][8] = faces[FACE_LEFT][8];
    
    faces[FACE_LEFT][6] = faces[FACE_BACK][6];
    faces[FACE_LEFT][7] = faces[FACE_BACK][7];
    faces[FACE_LEFT][8] = faces[FACE_BACK][8];
    
    faces[FACE_BACK][6] = faces[FACE_RIGHT][6];
    faces[FACE_BACK][7] = faces[FACE_RIGHT][7];
    faces[FACE_BACK][8] = faces[FACE_RIGHT][8];
    
    faces[FACE_RIGHT][6] = temp[0];
    faces[FACE_RIGHT][7] = temp[1];
    faces[FACE_RIGHT][8] = temp[2];
}
void Cube::moveL()
{
    rotateFaceClockwise(FACE_LEFT);
    char temp[3] = {
        faces[FACE_FRONT][0],
        faces[FACE_FRONT][3],
        faces[FACE_FRONT][6]
    };
    
    // Cycle: FRONT ← UP ← BACK ← DOWN ← FRONT
    faces[FACE_FRONT][0] = faces[FACE_UP][0];
    faces[FACE_FRONT][3] = faces[FACE_UP][3];
    faces[FACE_FRONT][6] = faces[FACE_UP][6];
    
    faces[FACE_UP][0] = faces[FACE_BACK][8];
    faces[FACE_UP][3] = faces[FACE_BACK][5];
    faces[FACE_UP][6] = faces[FACE_BACK][2];
    
    faces[FACE_BACK][8] = faces[FACE_DOWN][0];
    faces[FACE_BACK][5] = faces[FACE_DOWN][3];
    faces[FACE_BACK][2] = faces[FACE_DOWN][6];
    
    faces[FACE_DOWN][0] = temp[0];
    faces[FACE_DOWN][3] = temp[1];
    faces[FACE_DOWN][6] = temp[2];
}
void Cube::moveR()
{
    rotateFaceClockwise(FACE_RIGHT);
    char temp[3] = {
        faces[FACE_FRONT][2],
        faces[FACE_FRONT][5],
        faces[FACE_FRONT][8]
    };
    
    // Cycle: FRONT ← DOWN ← BACK ← UP ← FRONT
    faces[FACE_FRONT][2] = faces[FACE_DOWN][2];
    faces[FACE_FRONT][5] = faces[FACE_DOWN][5];
    faces[FACE_FRONT][8] = faces[FACE_DOWN][8];
    
    faces[FACE_DOWN][2] = faces[FACE_BACK][6];
    faces[FACE_DOWN][5] = faces[FACE_BACK][3];
    faces[FACE_DOWN][8] = faces[FACE_BACK][0];
    
    faces[FACE_BACK][6] = faces[FACE_UP][2];
    faces[FACE_BACK][3] = faces[FACE_UP][5];
    faces[FACE_BACK][0] = faces[FACE_UP][8];
    
    faces[FACE_UP][2] = temp[0];
    faces[FACE_UP][5] = temp[1];
    faces[FACE_UP][8] = temp[2];
}
void Cube::moveF()
{
    rotateFaceClockwise(FACE_FRONT);
    char temp[3] = {
        faces[FACE_UP][6],
        faces[FACE_UP][7],
        faces[FACE_UP][8]
    };
    
    // Cycle: UP ← LEFT ← DOWN ← RIGHT ← UP
    faces[FACE_UP][6] = faces[FACE_LEFT][8];
    faces[FACE_UP][7] = faces[FACE_LEFT][5];
    faces[FACE_UP][8] = faces[FACE_LEFT][2];
    
    faces[FACE_LEFT][2] = faces[FACE_DOWN][0];
    faces[FACE_LEFT][5] = faces[FACE_DOWN][1];
    faces[FACE_LEFT][8] = faces[FACE_DOWN][2];
    
    faces[FACE_DOWN][0] = faces[FACE_RIGHT][6];
    faces[FACE_DOWN][1] = faces[FACE_RIGHT][3];
    faces[FACE_DOWN][2] = faces[FACE_RIGHT][0];
    
    faces[FACE_RIGHT][0] = temp[0];
    faces[FACE_RIGHT][3] = temp[1];
    faces[FACE_RIGHT][6] = temp[2];
}
void Cube::moveB()
{
    rotateFaceClockwise(FACE_BACK);
    char temp[3] = {
        faces[FACE_UP][0],
        faces[FACE_UP][1],
        faces[FACE_UP][2]
    };
    
    // Cycle: UP ← RIGHT ← DOWN ← LEFT ← UP
    faces[FACE_UP][0] = faces[FACE_RIGHT][2];
    faces[FACE_UP][1] = faces[FACE_RIGHT][5];
    faces[FACE_UP][2] = faces[FACE_RIGHT][8];
    
    faces[FACE_RIGHT][2] = faces[FACE_DOWN][8];
    faces[FACE_RIGHT][5] = faces[FACE_DOWN][7];
    faces[FACE_RIGHT][8] = faces[FACE_DOWN][6];
    
    faces[FACE_DOWN][6] = faces[FACE_LEFT][0];
    faces[FACE_DOWN][7] = faces[FACE_LEFT][3];
    faces[FACE_DOWN][8] = faces[FACE_LEFT][6];
    
    faces[FACE_LEFT][0] = temp[2];
    faces[FACE_LEFT][3] = temp[1];
    faces[FACE_LEFT][6] = temp[0];
}


// PRIME MOVES (counterclockwise = 3 × clockwise)
void Cube::moveUPrime() { moveU(); moveU(); moveU(); }
void Cube::moveDPrime() { moveD(); moveD(); moveD(); }
void Cube::moveLPrime() { moveL(); moveL(); moveL(); }
void Cube::moveRPrime() { moveR(); moveR(); moveR(); }
void Cube::moveFPrime() { moveF(); moveF(); moveF(); }
void Cube::moveBPrime() { moveB(); moveB(); moveB(); }

// 180° MOVES
void Cube::moveU2() { moveU(); moveU(); }
void Cube::moveD2() { moveD(); moveD(); }
void Cube::moveL2() { moveL(); moveL(); }
void Cube::moveR2() { moveR(); moveR(); }
void Cube::moveF2() { moveF(); moveF(); }
void Cube::moveB2() { moveB(); moveB(); }

void Cube::executeMove(const std::string& move) {
    if      (move == "U")  this->move(U);
    else if (move == "U'") this->move(U_PRIME);
    else if (move == "U2") this->move(U2);
    else if (move == "D")  this->move(D);
    else if (move == "D'") this->move(D_PRIME);
    else if (move == "D2") this->move(D2);
    else if (move == "L")  this->move(L);
    else if (move == "L'") this->move(L_PRIME);
    else if (move == "L2") this->move(L2);
    else if (move == "R")  this->move(R);
    else if (move == "R'") this->move(R_PRIME);
    else if (move == "R2") this->move(R2);
    else if (move == "F")  this->move(F);
    else if (move == "F'") this->move(F_PRIME);
    else if (move == "F2") this->move(F2);
    else if (move == "B")  this->move(B);
    else if (move == "B'") this->move(B_PRIME);
    else if (move == "B2") this->move(B2);
    else std::cout << "[ERROR] Unknown move: " << move << "\n";
}

void Cube::move(int m) {
    switch(m) {
        case U: moveU(); break; case U_PRIME: moveUPrime(); break; case U2: moveU2(); break;
        case D: moveD(); break; case D_PRIME: moveDPrime(); break; case D2: moveD2(); break;
        case L: moveL(); break; case L_PRIME: moveLPrime(); break; case L2: moveL2(); break;
        case R: moveR(); break; case R_PRIME: moveRPrime(); break; case R2: moveR2(); break;
        case F: moveF(); break; case F_PRIME: moveFPrime(); break; case F2: moveF2(); break;
        case B: moveB(); break; case B_PRIME: moveBPrime(); break; case B2: moveB2(); break;
    }
}