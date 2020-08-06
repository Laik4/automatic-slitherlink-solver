#include "puzzle.h"
#include "solver.h"

using namespace std;

int main(int argc, char **argv){

    Puzzle p;
    p.load(argv[1]);
    Solver s(p);
    s.solve();
    return 0;
}
