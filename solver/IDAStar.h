#ifndef IDASTAR_H
#define IDASTAR_H
#include "PatternDatabase.h"
#include "../cube/Cube.h"
#include <vector>
#include <string>
#include <chrono>
using namespace std;

class IDAStar
{
private:
    vector<string> moveNames;
    int nodesExplored;
    vector<int> solutionIndices;
    
    // Optimized pruning tables
    int reverseMoveTable[18];
    bool canFollow[18][18];

    CornerPatternDatabase* cornerPDB;
    EdgePatternDatabase1* edgePDB1;
    EdgePatternDatabase2* edgePDB2;

    int search(Cube& cube, int g, int bound);
    chrono::time_point<chrono::steady_clock> searchStartTime;
    int timeoutSeconds;
    bool isTimeout();
    const int MAX_DEPTH = 20;

public:
    IDAStar();
    ~IDAStar();
    vector<string> solve(Cube& cube);
    int getNodesExplored() const;
    int heuristic(const Cube& cube) const;
};

#endif