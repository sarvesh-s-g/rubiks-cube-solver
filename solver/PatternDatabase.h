#ifndef PATTERN_DATABASE_H
#define PATTERN_DATABASE_H
#include "../cube/Cube.h"
#include <string>
#include <vector>
#include <cstdint>

using namespace std;
class PatternDatabase
{
protected:
    uint8_t* database;    //Hashing array
    uint32_t size;
    int defaultHeuristic;
    int factTable[13];
    virtual uint32_t getDatabaseIndex(const Cube& cube) const = 0;
    void generateBFS(int maxDepth);
    int factorial(int n) const;

public:
    PatternDatabase(uint32_t dbSize);
    virtual ~PatternDatabase();
    
    // Lookup heuristic value for a cube
    int lookup(const Cube& cube) const;
    void saveToFile(const string& filename) const;
    bool loadFromFile(const string& filename);
    uint32_t getSize() const { return size; }
};


// CORNER PATTERN DATABASE
class CornerPatternDatabase : public PatternDatabase
{
protected:
    // Extract corner permutation and orientation index from cube
    uint32_t getDatabaseIndex(const Cube& cube) const override;

public:
    CornerPatternDatabase();
};


// EDGE PATTERN DATABASE 1 (UF, UL, UB, UR, DF, DB)
class EdgePatternDatabase1 : public PatternDatabase
{
protected:
    uint32_t getDatabaseIndex(const Cube& cube) const override;
public:
    EdgePatternDatabase1();
};


// EDGE PATTERN DATABASE 2 (DL, DR, FL, FR, BL, BR)
class EdgePatternDatabase2 : public PatternDatabase
{
protected:
    uint32_t getDatabaseIndex(const Cube& cube) const override;
public:
    EdgePatternDatabase2();
};

#endif