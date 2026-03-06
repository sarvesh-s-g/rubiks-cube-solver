#include "PatternDatabase.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>
using namespace std;

// BASE PATTERN DATABASE CLASS
PatternDatabase::PatternDatabase(uint32_t dbSize)
{
    size = dbSize;
    database = new uint8_t[size];
    
    // Pre-calculate factorials
    factTable[0] = 1;
    for (int i = 1; i <= 12; i++) {
        factTable[i] = factTable[i - 1] * i;
    }
}

PatternDatabase::~PatternDatabase()
{
    delete[] database;
}

int PatternDatabase::factorial(int n) const
{
    return factTable[n];
}

int PatternDatabase::lookup(const Cube& cube) const
{
    uint32_t index = getDatabaseIndex(cube);
    if (index < size)
    {
        return database[index];
    }
    return defaultHeuristic;
}

void PatternDatabase::generateBFS(int maxDepth)
{
    cout << "[PDB] Starting BFS generation (max depth: " << maxDepth << ")...\n";
    cout << "[PDB] Initializing array with " << size << " elements...\n";
    
    // Fill the database initially with a value larger than any possible depth
    fill(database, database + size, 255);
    
    // Starting with solved cube
    Cube solved;
    uint32_t solvedIndex = getDatabaseIndex(solved);
    database[solvedIndex] = 0;
    queue<pair<Cube, int>> q;
    q.push({solved, 0});
    vector<string> moves = {
        "U", "U'", "U2",
        "D", "D'", "D2",
        "L", "L'", "L2",
        "R", "R'", "R2",
        "F", "F'", "F2",
        "B", "B'", "B2"
    };
    int lastDepth = 0;
    int entriesGenerated = 1;
    while (!q.empty())
    {
        Cube current = q.front().first;
        int depth = q.front().second;
        q.pop();
        
        if (depth > lastDepth)
        {
            lastDepth = depth;
            cout << "[PDB] Depth " << depth << " | Generated: " << entriesGenerated << " states\n";
        }
        
        if (depth >= maxDepth) continue;
        
        for (int m = 0; m < 18; m++)
        {
            Cube next = current;
            next.move(m);
            uint32_t index = getDatabaseIndex(next);
            if (database[index] == 255)
            {
                database[index] = depth + 1;
                entriesGenerated++;
                q.push({next, depth + 1});
            }
        }
    }
    // Replacing all remaining 255s with defaultHeuristic
    for (uint32_t i = 0; i < size; i++) {
        if (database[i] == 255) {
            database[i] = defaultHeuristic;
        }
    }
    
    cout << "[PDB] Generation complete! Filled entries: " << entriesGenerated << " / " << size << "\n";
}

void PatternDatabase::saveToFile(const string& filename) const
{
    cout << "[PDB] Saving to " << filename << "...\n";
    ofstream file(filename, ios::binary);
    if (!file.is_open())
    {
        cout << "[ERROR] Could not open file for writing\n";
        return;
    }
    file.write((char*)database, size * sizeof(uint8_t));
    file.close();
    cout << "[PDB] Saved " << size << " bytes successfully.\n";
}

bool PatternDatabase::loadFromFile(const string& filename)
{
    ifstream file(filename, ios::binary);
    if (!file.is_open()) return false;
    cout << "[PDB] Loading from " << filename << "...\n";
    file.read((char*)database, size * sizeof(uint8_t));
    if (file.fail()) {
        cout << "[PDB] Warning: File was partially corrupted or incomplete. Generating missing entries...\n";
        return false;
    }
    file.close();
    cout << "[PDB] Loaded " << size << " entries instantly.\n";
    return true;
}

// CORNER PATTERN DATABASE IMPLEMENTATION
// 88,179,840 states for 8 corners
CornerPatternDatabase::CornerPatternDatabase() : PatternDatabase(88179840)
{
    cout << "\n=== CORNER PATTERN DATABASE ===\n";
    defaultHeuristic = 11;  // Max depth is ~11 for corners
    
    if (loadFromFile("databases/corner_pdb_v2.dat"))
    {
        cout << "[Corner PDB] Loaded successfully from array format\n";
    }
    else
    {
        cout << "[Corner PDB] Not found. Generating 88.1M states...\n";
        cout << "[Corner PDB] This will take a few minutes...\n";
        generateBFS(11); 
        saveToFile("databases/corner_pdb_v2.dat");
    }
}

uint32_t CornerPatternDatabase::getDatabaseIndex(const Cube& cube) const
{
    char UP_color = cube.getSticker(0, 4);
    char DOWN_color = cube.getSticker(5, 4);
    char FRONT_color = cube.getSticker(2, 4);
    char BACK_color = cube.getSticker(4, 4);
    char LEFT_color = cube.getSticker(1, 4);
    char RIGHT_color = cube.getSticker(3, 4);

    struct CornerStruct { char stickers[3]; };
    CornerStruct corners[8] = {
        {{cube.getSticker(0, 8), cube.getSticker(2, 2), cube.getSticker(3, 0)}}, // 0: UFR
        {{cube.getSticker(0, 6), cube.getSticker(1, 2), cube.getSticker(2, 0)}}, // 1: UFL
        {{cube.getSticker(0, 0), cube.getSticker(4, 2), cube.getSticker(1, 0)}}, // 2: UBL
        {{cube.getSticker(0, 2), cube.getSticker(3, 2), cube.getSticker(4, 0)}}, // 3: UBR
        {{cube.getSticker(5, 2), cube.getSticker(2, 8), cube.getSticker(3, 6)}}, // 4: DFR
        {{cube.getSticker(5, 0), cube.getSticker(1, 8), cube.getSticker(2, 6)}}, // 5: DFL
        {{cube.getSticker(5, 6), cube.getSticker(4, 8), cube.getSticker(1, 6)}}, // 6: DBL
        {{cube.getSticker(5, 8), cube.getSticker(3, 8), cube.getSticker(4, 6)}}  // 7: DBR
    };

    int permutation[8];
    int orientation[8];
    
    for (int i = 0; i < 8; i++) {
        int twist = 0;
        for (int j = 0; j < 3; j++) {
            if (corners[i].stickers[j] == UP_color || corners[i].stickers[j] == DOWN_color) {
                twist = j;
                break;
            }
        }
        orientation[i] = twist;
        
        char c1 = corners[i].stickers[0];
        char c2 = corners[i].stickers[1];
        char c3 = corners[i].stickers[2];
        
        auto has = [&](char color) { return c1 == color || c2 == color || c3 == color; };

        if(has(UP_color) && has(FRONT_color) && has(RIGHT_color)) permutation[i] = 0;
        else if(has(UP_color) && has(FRONT_color) && has(LEFT_color)) permutation[i] = 1;
        else if(has(UP_color) && has(BACK_color) && has(LEFT_color)) permutation[i] = 2;
        else if(has(UP_color) && has(BACK_color) && has(RIGHT_color)) permutation[i] = 3;
        else if(has(DOWN_color) && has(FRONT_color) && has(RIGHT_color)) permutation[i] = 4;
        else if(has(DOWN_color) && has(FRONT_color) && has(LEFT_color)) permutation[i] = 5;
        else if(has(DOWN_color) && has(BACK_color) && has(LEFT_color)) permutation[i] = 6;
        else if(has(DOWN_color) && has(BACK_color) && has(RIGHT_color)) permutation[i] = 7;
        else permutation[i] = 0; // Fallback
    }
    
    uint32_t orientationIndex = 0;
    for (int i = 0; i < 7; i++) {
        orientationIndex = (orientationIndex * 3) + orientation[i];
    }
    
    uint32_t permutationIndex = 0;
    for (int i = 0; i < 7; i++) {
        int inversions = 0;
        for (int j = i + 1; j < 8; j++) {
            if (permutation[j] < permutation[i]) {
                inversions++;
            }
        }
        permutationIndex += inversions * factorial(7 - i);
    }
    return (permutationIndex * 2187) + orientationIndex;
}

// EDGE PATTERN DATABASE 1 (UF, UL, UB, UR, DF, DB)
EdgePatternDatabase1::EdgePatternDatabase1() : PatternDatabase(42577920)
{
    cout << "\n=== EDGE PATTERN DATABASE 1 ===\n";
    defaultHeuristic = 10;
    if (!loadFromFile("databases/edge_pdb1_v2.dat"))
    {
        cout << "[Edge PDB 1] Generating...\n";
        generateBFS(10);
        saveToFile("databases/edge_pdb1_v2.dat");
    }
}

uint32_t EdgePatternDatabase1::getDatabaseIndex(const Cube& cube) const
{
    char UP_color = cube.getSticker(0, 4);
    char DOWN_color = cube.getSticker(5, 4);
    char FRONT_color = cube.getSticker(2, 4);
    char BACK_color = cube.getSticker(4, 4);
    char LEFT_color = cube.getSticker(1, 4);
    char RIGHT_color = cube.getSticker(3, 4);

    // One pass over all 12 slots to identify target edges
    int permutation[6] = {0};
    int orientation[6] = {0};
    struct EdgeSlot { char s1, s2; };
    EdgeSlot slots[12] = {
        {cube.getSticker(0,7), cube.getSticker(2,1)}, {cube.getSticker(0,3), cube.getSticker(1,1)},
        {cube.getSticker(0,1), cube.getSticker(4,1)}, {cube.getSticker(0,5), cube.getSticker(3,1)},
        {cube.getSticker(5,1), cube.getSticker(2,7)}, {cube.getSticker(5,3), cube.getSticker(1,7)},
        {cube.getSticker(5,7), cube.getSticker(4,7)}, {cube.getSticker(5,5), cube.getSticker(3,7)},
        {cube.getSticker(2,3), cube.getSticker(1,5)}, {cube.getSticker(2,5), cube.getSticker(3,3)},
        {cube.getSticker(4,5), cube.getSticker(1,3)}, {cube.getSticker(4,3), cube.getSticker(3,5)}
    };

    // Target Edges for Database 1: UF, UL, UB, UR, DF, DB
    char targetP[6] = {UP_color, UP_color, UP_color, UP_color, DOWN_color, DOWN_color};
    char targetS[6] = {FRONT_color, LEFT_color, BACK_color, RIGHT_color, FRONT_color, BACK_color};

    for (int s = 0; s < 12; s++) {
        for (int t = 0; t < 6; t++) {
            if ((slots[s].s1 == targetP[t] && slots[s].s2 == targetS[t]) || 
                (slots[s].s1 == targetS[t] && slots[s].s2 == targetP[t])) {
                permutation[t] = s;
                orientation[t] = (slots[s].s1 == targetP[t]) ? 0 : 1;
                break;
            }
        }
    }

    uint32_t orientationIndex = 0;
    for (int i = 0; i < 6; i++) orientationIndex = (orientationIndex << 1) | orientation[i];
    
    uint32_t permutationIndex = 0;
    for (int i = 0; i < 6; i++) {
        int rank = permutation[i];
        for (int j = 0; j < i; j++) if (permutation[j] < permutation[i]) rank--;
        permutationIndex += rank * (factTable[11 - i] / 720);
    }
    return (permutationIndex << 6) | orientationIndex;  // <<6 means 2^6
}


// EDGE PATTERN DATABASE 2 (DL, DR, FL, FR, BL, BR)
EdgePatternDatabase2::EdgePatternDatabase2() : PatternDatabase(42577920)
{
    cout << "\n=== EDGE PATTERN DATABASE 2 ===\n";
    defaultHeuristic = 10;
    if (!loadFromFile("databases/edge_pdb2_v2.dat"))
    {
        cout << "[Edge PDB 2] Generating...\n";
        generateBFS(10);
        saveToFile("databases/edge_pdb2_v2.dat");
    }
}

uint32_t EdgePatternDatabase2::getDatabaseIndex(const Cube& cube) const
{
    char UP_color = cube.getSticker(0, 4);
    char DOWN_color = cube.getSticker(5, 4);
    char FRONT_color = cube.getSticker(2, 4);
    char BACK_color = cube.getSticker(4, 4);
    char LEFT_color = cube.getSticker(1, 4);
    char RIGHT_color = cube.getSticker(3, 4);

    int permutation[6] = {0};
    int orientation[6] = {0};
    struct EdgeSlot { char s1, s2; };
    EdgeSlot slots[12] = {
        {cube.getSticker(0,7), cube.getSticker(2,1)}, {cube.getSticker(0,3), cube.getSticker(1,1)},
        {cube.getSticker(0,1), cube.getSticker(4,1)}, {cube.getSticker(0,5), cube.getSticker(3,1)},
        {cube.getSticker(5,1), cube.getSticker(2,7)}, {cube.getSticker(5,3), cube.getSticker(1,7)},
        {cube.getSticker(5,7), cube.getSticker(4,7)}, {cube.getSticker(5,5), cube.getSticker(3,7)},
        {cube.getSticker(2,3), cube.getSticker(1,5)}, {cube.getSticker(2,5), cube.getSticker(3,3)},
        {cube.getSticker(4,5), cube.getSticker(1,3)}, {cube.getSticker(4,3), cube.getSticker(3,5)}
    };

    // Target Edges for Database 2: DL, DR, FL, FR, BL, BR
    char targetP[6] = {DOWN_color, DOWN_color, FRONT_color, FRONT_color, BACK_color, BACK_color};
    char targetS[6] = {LEFT_color, RIGHT_color, LEFT_color, RIGHT_color, LEFT_color, RIGHT_color};

    for (int s = 0; s < 12; s++) {
        for (int t = 0; t < 6; t++) {
            if ((slots[s].s1 == targetP[t] && slots[s].s2 == targetS[t]) || 
                (slots[s].s1 == targetS[t] && slots[s].s2 == targetP[t])) {
                permutation[t] = s;
                orientation[t] = (slots[s].s1 == targetP[t]) ? 0 : 1;
                break;
            }
        }
    }

    uint32_t orientationIndex = 0;
    for (int i = 0; i < 6; i++) orientationIndex = (orientationIndex << 1) | orientation[i];
    
    uint32_t permutationIndex = 0;
    for (int i = 0; i < 6; i++) {
        int rank = permutation[i];
        for (int j = 0; j < i; j++) if (permutation[j] < permutation[i]) rank--;
        permutationIndex += rank * (factTable[11 - i] / 720);
    }
    return (permutationIndex << 6) | orientationIndex;
}
