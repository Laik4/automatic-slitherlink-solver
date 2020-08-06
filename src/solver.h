#pragma once

#include "puzzle.h"

using namespace std;

class Solver{
private:
    Puzzle puzzle;

    bool satisfied();

    void prune_zero();
    int extend_decided();
    int decide_by_prohibited();
    void prune_deadend();
    int satisfy_constraint();

    void show_max_degree();
    void show_status();

public:
    Solver(Puzzle p);
    void solve(void);

};
