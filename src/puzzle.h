#pragma once

#include <string>
#include <vector>
#include "vertex.h"
#include "edge.h"

using namespace std;

class Puzzle{
private:
public:
    vector<vector<Vertex>> vertex;
    vector<vector<Edge>> edge;
    vector<vector<int>> constraint;
    int rows;
    int cols;
    int load(string filename);
    void show(void);

    Edge get_edge(int r1, int c1, int r2, int c2);
    Edge get_edge(int r, int c, int direction);
    Edge get_edge_v(int r, int c, int direction);
    
    void set_edge(int r1, int c1, int r2, int c2, int status);
    void set_edge(int r, int c, int direction, int status);
    void set_edge_v(int r, int c, int direction, int status);
    
};


