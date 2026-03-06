#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <limits>
#include "vision/Camera.h"
#include "solver/IDAStar.h"
#include "cube/Cube.h"

using namespace std;


// Face scanning function

vector<char> scanFace(Camera& camera, const string& faceName)
{
    cout << "\n........................................\n";
    cout << "  SCANNING " << faceName << " FACE\n";
    cout << "........................................\n";
    cout << "Instructions:\n";
    cout << "1. Point camera at the " << faceName << " face\n";
    cout << "2. Make sure all 9 stickers are visible in the grid\n";
    cout << "3. ***Click on camera window***\n";
    cout << "3. Press SPACE when colors look correct\n";
    cout << "4. Press ESC to cancel\n";
    cout << "........................................\n\n";
    
    vector<char> scannedColors;
    
    while (true)
    {
        // Capture and show frame
        if (!camera.captureFrame())
        {
            cout << "[ERROR] Failed to capture frame!\n";
            break;
        }
        
        // Detect colors in real-time
        vector<char> colors = camera.detectColors();
        camera.showFrame();
        
        int key = camera.waitKey(1);
        
        // ESC - Cancel scanning
        if (key == 27)
        {
            cout << "[WARNING] Scan cancelled!\n";
            return vector<char>();  
        }
        
        // SPACE - Accept this face
        if (key == 32)
        {
            scannedColors = colors;
            
            cout << "[SUCCESS] " << faceName << " face scanned:\n";
            cout << "-------------\n";
            
            int index = 0;
            for (int row = 0; row < 3; row++)
            {
                cout << "| ";
                for (int col = 0; col < 3; col++)
                {
                    cout << scannedColors[index++] << " | ";
                }
                cout << "\n-------------\n";
            }
            
            break;
        }
    }
    
    return scannedColors;
}

// mannual 
vector<char> manualFaceEntry(const string& faceName)
{
    cout << "\n.........................................\n";
    cout << ". MANUAL ENTRY FOR " << faceName << " FACE.\n";
    cout << ".........................................\n";
    cout << "Type nine colour letters (W R B O G Y) separated by spaces or\n";
    cout << "as a single string.  Press Enter when done.\n";

    vector<char> colors(9);
    for (int i = 0; i < 9; ++i)
    {
        char c;
        cin >> c;
        colors[i] = c;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "[INFO] you entered:\n-------------\n";
    int idx = 0;
    for (int r = 0; r < 3; ++r)
    {
        cout << "| ";
        for (int c = 0; c < 3; ++c)
            cout << colors[idx++] << " | ";
        cout << "\n-------------\n";
    }

    return colors;
}

// MAIN FUNCTION
int main()
{
    char choice = 'y';
    while(choice=='y'||choice=='Y')
    {
    cout << "\n.........................................\n";
    cout << ".   RUBIK'S CUBE SCANNER & SOLVER       .\n";
    cout << ".........................................\n";
    
    Cube cube;
    cout << "[INFO] Initial state: Solved\n";
    
    cout << "Choose input method:\n";
    cout << "  1) Scan each face with the camera\n";
    cout << "  2) Enter colours manually from keyboard\n";
    cout << "  3) Random Scramble (Test Mode)\n";
    cout << "  4) Exit\n";
    cout << "Selection (1/2/3/4): ";
    int mode = 1;
    cin >> mode;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    Camera camera; 
    
    if(mode == 1)
    {
        if (!camera.start()) {
            cout << "[ERROR] Camera failed. Switching to Manual Entry...\n";
            mode = 2;
        }
    }
    
    //Scan all 6 faces
    string faceNames[6] = {"UP", "LEFT", "FRONT", "RIGHT", "BACK", "DOWN"};
    int faceIndices[6] = {FACE_UP, FACE_LEFT, FACE_FRONT, FACE_RIGHT, FACE_BACK, FACE_DOWN};
    
    if(mode == 1)
    {
        cout << "\n.........................................\n";
        cout << ".         SCANNING ALL 6 FACES          .\n";
        cout << ".........................................\n";
        for (int i = 0; i < 6; i++) {
            vector<char> faceColors = scanFace(camera, faceNames[i]);
            if (faceColors.empty()) return -1;
            cube.setFace(faceIndices[i], faceColors.data());
        }
    }
    else if(mode == 2)
    {
        cout << "\n.........................................\n";
        cout << ".        MANUAL ENTRY OF FACES          .\n";
        cout << ".........................................\n";
        for (int i = 0; i < 6; ++i) {
            vector<char> faceColors = manualFaceEntry(faceNames[i]);
            cube.setFace(faceIndices[i], faceColors.data());
        }
    }
    else if(mode == 3)
    {
        cout << "\n.........................................\n";
        cout << ".         RANDOM SCRAMBLE MODE          .\n";
        cout << ".........................................\n";
        
        int numMoves = 0;
        while (numMoves < 1 || numMoves > 13) {
            cout << "\nEnter number of random moves (1-13): ";
            cin >> numMoves;
            if (numMoves > 13) {
                cout << "[NOTE] IDA* solver works optimally for up to 13 moves.\n";
                cout << "       Please enter a value between 1 and 13.\n";
            }
        }
        
        srand(time(0));
        string scrambleMoves[] = {
            "U", "U'", "U2", "D", "D'", "D2",
            "L", "L'", "L2", "R", "R'", "R2",
            "F", "F'", "F2", "B", "B'", "B2"
        };
        string lastFace = "";
        
        cout << "\nApplying Scramble: ";
        for(int i=0; i<numMoves; i++) {
            string m;
            do { m = scrambleMoves[rand() % 18]; } while(!lastFace.empty() && m[0] == lastFace[0]);
            lastFace = m;
            cube.executeMove(m);
            cout << m << " ";
        }
        cout << "\n\n[SUCCESS] Cube scrambled.\n";
    }
    else
    {
        return 0;
    }
     camera.stop();
     cube.print();
    
    int counts[256] = {0};
    for(int f=0; f<6; f++) {
        for(int i=0; i<9; i++) counts[(unsigned char)cube.getSticker(f, i)]++;
    }
    bool invalid = false;
    char colors[] = {'W', 'G', 'B', 'O', 'Y', 'R'};
    for(char c : colors) {
        if(counts[(unsigned char)c] != 9) {
            cout << "[ERROR] Invalid Cube: " << c << " appears " << counts[(unsigned char)c] << " times (should be 9)\n";
            invalid = true;
        }
    }
    if(invalid) {
        cout << "[CRITICAL] Fix your scan/entry before solving.\n";
        return -1;
    }

    // solver
    cout << "\n.........................................\n";
    cout << ".            SOLVING CUBE               .\n";
    cout << ".........................................\n";
    
    IDAStar solver;
    int hVal = solver.heuristic(cube);
    cout << "[INFO] Initial heuristic distance: " << hVal << "\n";
    
    if (hVal >= 18) {
        cout << "[WARNING] Distance is very high (" << hVal << "). Check your entries for errors.\n";
    }

    cout << "\nWould you like to solve the cube? (y/n): ";

        char solveChoice;
        cin >> solveChoice;

        if (solveChoice == 'y' || solveChoice == 'Y')
        {
            Cube cubeClone = cube; 
            vector<string> solution = solver.solve(cubeClone);
            
            if (!solution.empty())
            {
                cout << "\n......................................\n";
                cout << "           SOLUTION FOUND!\n";
                cout << "........................................\n";
                cout << "\nMoves: ";
                for (const string& move : solution) cout << move << " ";
                cout << "\n\nTotal moves: " << solution.size() << "\n";
                
                cout << "\nApply solution to verify? (y/n): ";
                char verifyChoice;
                cin >> verifyChoice;
                
                if (verifyChoice == 'y' || verifyChoice == 'Y')
                {
                    Cube verCube = cube;
                    for (const string& m : solution)
                    {
                        verCube.executeMove(m);
                    }
                    
                    if (verCube.isSolved())
                    {
                        cout << "\n[OK] Solution verified! Cube is solved!\n";
                    }
                    else
                    {
                        cout << "\n[FAIL] Solution could not be verified.\n";
                    }
                }
            }
        }
        cout<<"Do you want to solve another cube (y/n) \n";
        cin>>choice;
        if(choice=='n' || choice=='N')
        {
             cout << "\nPress Enter to exit...\n";
             cin.get();
             cin.get();
             break;
        }
   
    }
    return 0;
}
