#include "IDAStar.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <string>
#include <chrono>
#include <cstdlib>
#include <ctime>
using namespace std;

IDAStar::IDAStar()
{
    moveNames = {
        "U", "U'", "U2", "D", "D'", "D2",
        "L", "L'", "L2", "R", "R'", "R2",
        "F", "F'", "F2", "B", "B'", "B2"
    };

    reverseMoveTable[U] = U_PRIME; reverseMoveTable[U_PRIME] = U; reverseMoveTable[U2] = U2;
    reverseMoveTable[D] = D_PRIME; reverseMoveTable[D_PRIME] = D; reverseMoveTable[D2] = D2;
    reverseMoveTable[L] = L_PRIME; reverseMoveTable[L_PRIME] = L; reverseMoveTable[L2] = L2;
    reverseMoveTable[R] = R_PRIME; reverseMoveTable[R_PRIME] = R; reverseMoveTable[R2] = R2;
    reverseMoveTable[F] = F_PRIME; reverseMoveTable[F_PRIME] = F; reverseMoveTable[F2] = F2;
    reverseMoveTable[B] = B_PRIME; reverseMoveTable[B_PRIME] = B; reverseMoveTable[B2] = B2;

    for (int i = 0; i < 18; i++) {
        for (int j = 0; j < 18; j++) {
            canFollow[i][j] = true;
            if (i / 3 == j / 3) canFollow[i][j] = false;
            // Opposite faces canonical ordering (U < D, L < R, F < B)
            if (i / 3 == (D / 3) && j / 3 == (U / 3)) canFollow[i][j] = false;
            if (i / 3 == (R / 3) && j / 3 == (L / 3)) canFollow[i][j] = false;
            if (i / 3 == (B / 3) && j / 3 == (F / 3)) canFollow[i][j] = false;
        }
    }

    nodesExplored = 0;
    cornerPDB = new CornerPatternDatabase();
    edgePDB1 = new EdgePatternDatabase1();
    edgePDB2 = new EdgePatternDatabase2();
}

IDAStar::~IDAStar()
{
    delete cornerPDB;
    delete edgePDB1;
    delete edgePDB2;
}

int IDAStar::heuristic(const Cube& cube) const
{
    if (cube.isSolved()) return 0;
    int h1 = cornerPDB->lookup(cube);
    int h2 = edgePDB1->lookup(cube);
    int h3 = edgePDB2->lookup(cube);
    return max({h1, h2, h3});
}

int IDAStar::getNodesExplored() const { return nodesExplored; }

vector<string> IDAStar::solve(Cube& cube)
{
    cout << "\n--------------------------------------\n";
    cout << "|   STARTING OPTIMIZED IDA* SOLVER   |\n";
    cout << "--------------------------------------\n";
    
    nodesExplored = 0;
    timeoutSeconds = 20;
    searchStartTime = chrono::steady_clock::now();
    solutionIndices.clear();

    if (cube.isSolved()) return vector<string>();
    
    int bound = heuristic(cube);
    cout << "[INFO] Initial heuristic: " << bound << "\n\n";

    while (bound <= MAX_DEPTH) {
        int result = search(cube, 0, bound);
        
        if (result == 0) {
            Cube verifyCube = cube;
            for (int mIndex : solutionIndices)
                verifyCube.move(mIndex);
            
            if (verifyCube.isSolved()) {
                cout << "\n\n[SUCCESS] Optimal solution found!\n";
                vector<string> finalSolution;
                for (int idx : solutionIndices) finalSolution.push_back(moveNames[idx]);
                return finalSolution;
            } else {
                cout << "\n[ERROR] Internal solver error. Please re-scramble and try again.\n";
                return {};
            }
        }
        
        if (result == -1) {
            cout << "\n[INFO] Search timed out after " << timeoutSeconds << " seconds.\n";
            cout << "[INFO] This scramble is too deep for the solver.\n";
            cout << "[INFO] Please use a scramble with at most 13 moves.\n";
            return {};
        }

        if (result > bound) bound = result;
        else bound++;
    }
    
    cout << "[INFO] Max search depth reached without solution.\n";
    cout << "[INFO] Please use a scramble with at most 13 moves.\n";
    return {};
}
        
bool IDAStar::isTimeout()
{
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::seconds>(now - searchStartTime);
    return elapsed.count() >= timeoutSeconds;
}

int IDAStar::search(Cube& cube, int g, int bound)
{
    nodesExplored++;
    
    // Checking timeout every 64k nodes using bitmask
    if ((nodesExplored & 0xFFFF) == 0) {
        if (isTimeout()) return -1;
    }
    
    int h = heuristic(cube);
    int f = g + h;
    if (f > bound) return f;
    if (h == 0 && cube.isSolved()) return 0;

    int min_f = 99;
    int lastMoveIdx = solutionIndices.empty() ? -1 : solutionIndices.back();

    for (int m = 0; m < 18; m++) {
        if (lastMoveIdx != -1 && !canFollow[lastMoveIdx][m]) continue;

        cube.move(m);
        solutionIndices.push_back(m);
        
        int res = search(cube, g + 1, bound);
        cube.move(reverseMoveTable[m]);
        if (res == 0) {
            return 0;
        }

        if (res == -1) {
            return -1;
        }

        if (res < min_f) min_f = res;

        solutionIndices.pop_back();
        
        if ((nodesExplored & 0xFFFFF) == 0) {
            cout << "[SEARCH] Nodes: " << nodesExplored / 1000000 << "M    (Time: " 
                 << chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - searchStartTime).count() 
                 << "s)                \r";
            cout.flush();
        }
    }
    return min_f;
}