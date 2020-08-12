#include <stack>
#include <utility>
#include <iostream>

#include "puzzle.h"
#include "solver.h"
#include "const.h"

int Solver::trace_loop(){
// TODO: add this function to solver.h
    vector<vector<bool>> visited(puzzle.rows+1, vector<bool>(puzzle.cols+1, false));

    // select init edge from unvisited one
    // trace loop until come back to init edge or reach deadend
    // continue until visit all edges
    


}

int Solver::satisfied(){

    // check if degree <= 2 
    for(int r = 0; r < puzzle.rows; ++r){
        for(int c = 0; c < puzzle.cols; ++c){
            if(puzzle.vertex[r][c].degree >= 3){
                return UNSATISFIED;
            }
        }
    }

    bool all_satisfied = true;

    // check if constraints are satisfied
    for(int r = 0; r < puzzle.rows; ++r){
        for(int c = 0; c < puzzle.cols; ++c){
            if(puzzle.constraint[r][c] == 0){
                continue;
            }
            int prohibited = 0;
            int decided = 0;
            for(int i = 0; i < 4; ++i){
                switch(puzzle.get_edge(r, c, i)){
                    case PROHIBITED:
                        ++prohibited;
                        break;
                    case DECIDED:
                        ++decided;
                        break;
                }
                if(all_satisified and puzzle.constraint[r][c] != decided){
                    all_satisfied = false;
                }
                if(puzzle.constraint[r][c] < decided or 4-puzzle.constraint[r][c] < prohibited){
                    return UNSATISFIED;
                }
            }
        }
    }

    // check if there's any loop
    // if all_satisfied == false and there's only 1 loop, the puzzle is solved.


    


    // otherwise, the constraints are satisfied so far.
    return SATISFIED;
}

void Solver::prune_zero(){
    for(int r = 0; r < puzzle.rows; ++r){
        for(int c = 0; c < puzzle.cols; ++c){
            if(puzzle.constraint[r][c] == 0){
                for(int i = 0; i < 4; ++i){
                    puzzle.set_edge(r, c, i, PROHIBITED);
                }
            }
        }
    }
}

int Solver::extend_decided(){
    for(int r = 0; r <= puzzle.rows; ++r){
        for(int c = 0; c <= puzzle.cols; ++c){
            int prohibited = 0, decided = 0;
            for(int i = 0; i < 4; ++i){
                int e = puzzle.get_edge_v(r, c, i);
                switch(e){
                    case PROHIBITED:
                        ++prohibited;
                        break;
                    case DECIDED:
                        ++decided;
                        break;
                }
            }
            switch(decided){
                case 0:
                    break;
                case 1:
                    // When maxdegree is 2 and degree is 1, an edge must be decided
                    if(puzzle.vertex[r][c].max_degree == 2){
                        for(int i = 0; i < 4; ++i){
                            if(puzzle.get_edge_v(r, c, i) == PENDING){
                                puzzle.set_edge_v(r, c, i, DECIDED);
                            }
                        }
                    }
                    break;

                // When there are 2 edges around the vertex, the others must be prohibited
                case 2:
                    for(int i = 0; i < 4; ++i){
                        if(puzzle.get_edge_v(r, c, i) != DECIDED){
                            puzzle.set_edge_v(r, c, i, PROHIBITED);
                        }
                    }
                    break;
                default:
                    return -1;
            }
        }
    }
    return 0;
}
int Solver::decide_by_prohibited(){
    for(int r = 0; r < puzzle.rows; ++r){
        for(int c = 0; c < puzzle.cols; ++c){
            // count the number of prohibited edges around (r, c)
            int prohibited_num = 0;
            for(int i = 0; i < 4; ++i){
                if(puzzle.get_edge(r, c, i) == PROHIBITED){
                    ++prohibited_num;
                }
            }
            if(puzzle.constraint[r][c] == 4-prohibited_num){
                for(int i = 0; i < 4; ++i){
                    if(puzzle.get_edge(r, c, i) == PENDING){
                        puzzle.set_edge(r, c, i, DECIDED);
                    }
                }
            }else if(puzzle.constraint[r][c] > 4-prohibited_num){
                return -1;
            }
        }
    }
    return 0;
}

void Solver::prune_deadend(){
    stack<pair<int, int>> q;

    for(int r = 0; r <= puzzle.rows; ++r){
        for(int c = 0; c <= puzzle.cols; ++c){
            if(puzzle.vertex[r][c].max_degree == DECIDED){
                --puzzle.vertex[r][c].max_degree;
                q.emplace(r, c);
            }
        }
    }

    int d[5] = {1, 0, -1, 0, 1};
    while(!q.empty()){
        auto [r, c] = q.top(); q.pop();
        for(int i = 0; i < 4; ++i){
            int R = r+d[i], C = c+d[i+1];
            bool out_of_range = R < 0 or puzzle.rows < R
                             or C < 0 or puzzle.cols < C;
            if(out_of_range) continue;

            bool line_prohibited = puzzle.get_edge(r, c, R, C) == PROHIBITED;
            if(line_prohibited) continue;
            if(puzzle.vertex[R][C].max_degree > 0){
                --puzzle.vertex[R][C].max_degree;
                puzzle.set_edge(r, c, R, C, PROHIBITED);
            }
            bool degree_is_1 = puzzle.vertex[R][C].max_degree == 1;
            if(degree_is_1){
                q.emplace(R, C);
            }
        }
    }
}



int Solver::satisfy_constraint(){
    for(int r = 0; r < puzzle.rows; ++r){
        for(int c = 0; c < puzzle.cols; ++c){
            int prohibited = 0, decided = 0;
            for(int i = 0; i < 4; ++i){
                int e = puzzle.get_edge(r, c, i);
                switch(e){
                    case PROHIBITED:
                        ++prohibited;
                        break;
                    case DECIDED:
                        ++decided;
                        break;
                }
            }
            bool satisfiable = puzzle.constraint[r][c] >= decided or puzzle.constraint[r][c] <= 4-prohibited;
            if(!satisfiable) return -1;
            if(puzzle.constraint[r][c] == decided){
                for(int i = 0; i < 4; ++i){
                    if(puzzle.get_edge(r, c, i) == PENDING){
                        puzzle.set_edge(r, c, i, PROHIBITED);
                    }

                }
            }
            if(puzzle.constraint[r][c] == 4-prohibited){
                for(int i = 0; i < 4; ++i){
                    if(puzzle.get_edge(r, c, i) == PENDING){
                        puzzle.set_edge(r, c, i, DECIDED);
                    }
                }
            }
        }
    }
    return 0;
}

void Solver::show_max_degree(){
    for(int r = 0; r <= puzzle.rows; ++r){
        for(int c = 0; c <= puzzle.cols; ++c){
            cout << puzzle.vertex[r][c].max_degree;
            if(c < puzzle.cols)  cout << "───";
        }
        cout << '\n';
        if(r < puzzle.rows){
            for(int c = 0; c <= puzzle.cols; ++c){
                cout << "│   ";
            }
        }
        cout << '\n';
    }
}
void Solver::show_status(){
    cout << "┌";
    for(int c = 0; c < puzzle.cols; ++c){
        int e = static_cast<int>(puzzle.get_edge(0, c, UP));

        switch(e){
            case PROHIBITED:
                cout << " X ";
                break;
            case PENDING:
                cout << "─?─";
                break;
            case DECIDED:
                cout << "─O─";
                break;
        }
        cout << ((c < puzzle.cols-1) ? "┬" : "┐");
    }
    cout << '\n';

    for(int r = 0; r < puzzle.rows; ++r){
        int e = static_cast<int>(puzzle.get_edge(r, 0, LEFT));
        switch(e){
            case PROHIBITED:
                cout << "X";
                break;
            case PENDING:
                cout << "?";
                break;
            case DECIDED:
                cout << "O";
                break;
        }

        for(int c = 0; c < puzzle.cols; ++c){
            cout << "   ";
            e = static_cast<int>(puzzle.get_edge(r, c, RIGHT));
            switch(e){
                case PROHIBITED:
                    cout << "X";
                    break;
                case PENDING:
                    cout << "?";
                    break;
                case DECIDED:
                    cout << "O";
                    break;
            }
        }
        cout << '\n';

        cout << ((r < puzzle.rows-1) ? "├" : "└");
        for(int c = 0; c < puzzle.cols; ++c){
            e = static_cast<int>(puzzle.get_edge(r, c, DOWN));
            switch(e){
                case PROHIBITED:
                    cout << " X ";
                    break;
                case PENDING:
                    cout << "─?─";
                    break;
                case DECIDED:
                    cout << "─O─";
                    break;
            }

            if(r < puzzle.rows-1){
                cout << ((c < puzzle.cols-1) ? "┼" : "┤");
            }else{
                cout << ((c < puzzle.cols-1) ? "┴" : "┘");
            }
        }
        cout << '\n';
    }
}


Solver::Solver(Puzzle p){
    this->puzzle = p;
}
void Solver::solve(){
    int satisfiable = 0;
    this->prune_zero();
    for(int i = 0; i < 5; ++i){
        this->prune_deadend();
        satisfiable |= this->decide_by_prohibited();
        satisfiable |= this->extend_decided();
        satisfiable |= this->satisfy_constraint();

        if(satisfiable < 0){
            cout << "This puzzle is unsatisfiable." << '\n';
            return;
        }
    }
    this->puzzle.show();
    this->show_max_degree();
    this->show_status();
}

