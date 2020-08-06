#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "puzzle.h"
#include "edge.h"
#include "vertex.h"
#include "const.h"

using namespace std;

int Puzzle::load(string filename){
    ifstream ifs(filename);
    if(!ifs){
        cerr << "Can't open " << filename << ". \n";
        return 1;
    }

    string buf;
    ifs >> this->rows >> this->cols;
    getline(ifs, buf);
    this->constraint.resize(this->rows, vector<int>(this->cols, -1));
    this->vertex.resize(this->rows+1, vector<Vertex>(this->cols+1));
    this->edge.resize(2*this->rows+1, vector<Edge>(this->cols+1));

    // init vertex
    for(int r = 0; r <= this->rows; ++r){
        for(int c = 0; c <= this->cols; ++c){
            int max_deg = 4;
            if(r == 0 or r == this->rows) --max_deg;
            if(c == 0 or c == this->cols) --max_deg;
            this->vertex[r][c].max_degree = max_deg;
            this->vertex[r][c].degree = 0;
        }
    }

    // init edge
    for(int r = 0; r <= 2*this->rows; ++r){
        for(int c = 0; c <= this->cols; ++c){
            this->edge[r][c].status = 0;
        }
    }

    // load constraints
    for(int r = 0; r < this->rows; ++r){
        for(int c = 0; c < this->cols; ++c){
            char buf;
            ifs >> buf;
            if(buf == 'x'){
                continue;
            }else{
                assert('0' <= buf and buf <= '3');
                this->constraint[r][c] = buf-'0'; 
            }
        }
    }

    return 0;
}

void Puzzle::show(void){
    for(int i = 0; i <= this->rows; ++i){
        for(int j = 0; j <= this->cols; ++j){
            cerr << this->vertex[i][j].degree << ' ';
        }
        cerr << '\n';
    }

    if(this->vertex[0][0].degree == 2){
        cout << "\e[31m";
    }
    cout << "┌";
    cout << "\e[0m";

    for(int c = 0; c < this->cols; ++c){
        int e = static_cast<int>(this->get_edge(0, c, UP));

        switch(e){
            case PROHIBITED:
                cout << " \e[2mx\e[0m ";
                break;
            case PENDING:
                cout << "╌╌╌";
                break;
            case DECIDED:
                cout << "\e[31m───\e[0m";
                break;
        }
        if(this->vertex[0][c+1].degree == 2){
            cout << "\e[31m";
        }
        cout << ((c < this->cols-1) ? "┬" : "┐");
        cout << "\e[0m";
    }
    cout << '\n';

    for(int r = 0; r < this->rows; ++r){
        int e = static_cast<int>(this->get_edge(r, 0, LEFT));

        switch(e){
            case PROHIBITED:
                cout << "\e[2m";
                cout << "x";
                cout << "\e[0m";
                break;
            case PENDING:
                cout << "┆";
                break;
            case DECIDED:
                cout << "\e[31m│\e[0m";
                break;
        }

        for(int c = 0; c < this->cols; ++c){
            cout << " ";
            if(this->constraint[r][c] >= 0){
                cout << this->constraint[r][c];
            }else{
                cout << " ";
            }
            cout << " ";

            e = static_cast<int>(this->get_edge(r, c, RIGHT));
            switch(e){
                case PROHIBITED:
                    cout << "\e[2m";
                    cout << "x";
                    cout << "\e[0m";
                    break;
                case PENDING:
                    cout << "┆";
                    break;
                case DECIDED:
                    cout << "\e[31m│\e[0m";
                    break;
            }
        }
        cout << '\n';

        if(this->vertex[r+1][0].degree == 2){
            cout << "\e[31m";
        }
        cout << ((r < this->rows-1) ? "├" : "└");
        cout << "\e[0m";

        for(int c = 0; c < this->cols; ++c){
            e = static_cast<int>(this->get_edge(r, c, DOWN));
            switch(e){
                case PROHIBITED:
                    cout << " \e[2mx\e[0m ";
                    break;
                case PENDING:
                    cout << "╌╌╌";
                    break;
                case DECIDED:
                    cout << "\e[31m───\e[0m";
                    break;
            }

            if(this->vertex[r+1][c+1].degree == 2){
                cout << "\e[31m";
            }
            if(r < this->rows-1){
                cout << ((c < this->cols-1) ? "┼" : "┤");
            }else{
                cout << ((c < this->cols-1) ? "┴" : "┘");
            }
            cout << "\e[0m";
        }
        cout << '\n';
    }


}

// get the status of an edge which connects (r1, c1) and (r2, c2)
Edge Puzzle::get_edge(int r1, int c1, int r2, int c2){
    assert(r1 == r2 or c1 == c2);
    bool out_of_range = r1 < 0 or this->rows < r1
                     or r2 < 0 or this->rows < r2
                     or c1 < 0 or this->cols < c1
                     or c2 < 0 or this->cols < c2;
    if(out_of_range)  return Edge(-1);
    if(r1 > r2) swap(r1, r2);
    if(c1 > c2) swap(c1, c2);
    return this->edge[2*r1+(c1==c2)][c1].status;
}

// get the status of an edge by specifying the position of a constraint and where edge locates
Edge Puzzle::get_edge(int r, int c, int direction){
    bool out_of_range = r < 0 or this->rows <= r 
                     or c < 0 or this->cols <= c;
    if(out_of_range)  return Edge(-1);
    Edge e;
    switch(direction){
        case UP:
            e = this->edge[2*r][c].status;
            break;
        case LEFT:
            e = this->edge[2*r+1][c].status;
            break;
        case DOWN:
            e = this->edge[2*r+2][c].status;
            break;
        case RIGHT:
            e = this->edge[2*r+1][c+1].status;
            break;
    }
    return e;
}

// get the status of an edge by specifying the position of a vertex and where edge locates
Edge Puzzle::get_edge_v(int r, int c, int direction){
    bool out_of_range = r < 0 or this->rows < r 
                     or c < 0 or this->cols < c;
    if(out_of_range)  return Edge(-1);
    Edge e;
    switch(direction){
        case UP:
            if(r == 0) return Edge(-1);
            e = this->edge[2*r-1][c].status;
            break;
        case LEFT:
            if(c == 0) return Edge(-1);
            e = this->edge[2*r][c-1].status;
            break;
        case DOWN:
            if(r == this->rows) return Edge(-1);
            e = this->edge[2*r+1][c].status;
            break;
        case RIGHT:
            if(c == this->cols) return Edge(-1);
            e = this->edge[2*r][c].status;
            break;
    }
    return e;

}

// set the status of an edge by specifying two vertices (r1, c1) and (r2, c2) 
void Puzzle::set_edge(int r1, int c1, int r2, int c2, int status){
    assert(PROHIBITED <= status and status <= DECIDED);
    assert(r1 == r2 or c1 == c2);
    bool out_of_range = r1 < 0 or this->rows < r1 
                     or r2 < 0 or this->rows < r2 
                     or c1 < 0 or this->cols < c1  
                     or c2 < 0 or this->cols < c2;
    if(out_of_range)  return;
    if(r1 > r2) swap(r1, r2);
    if(c1 > c2) swap(c1, c2);
    if(r1 == r2){
        if(this->edge[2*r1][c1].status == status) return;
        this->edge[2*r1][c1].status = status;
    }
    if(c1 == c2){
        if(this->edge[2*r1+1][c1].status == status) return;
        this->edge[2*r1+1][c1].status = status;
    }

}

// set the status of an edge by specifying the position of a constraint and where edge locates
void Puzzle::set_edge(int r, int c, int direction, int status){
    // direction   [0:up] [1:left] [2:down] [3:left]
    assert(PROHIBITED <= status and status <= DECIDED);
    bool out_of_range = r < 0 or this->rows <= r 
                     or c < 0 or this->cols <= c;
    if(out_of_range)  return;

    // Update the status of an edge and the max_degree of each vertex
    int d_max_degree = (status == PROHIBITED) ? -1 : 0;
    int d_degree = (status == DECIDED) ? 1 : 0;
    switch(direction){
        case UP:
            this->edge[2*r][c].status = status;
            this->vertex[r][c].max_degree += d_max_degree;
            this->vertex[r][c+1].max_degree += d_max_degree;
            this->vertex[r][c].degree += d_degree;
            this->vertex[r][c+1].degree += d_degree;
            break;
        case LEFT:
            this->edge[2*r+1][c].status = status;
            this->vertex[r][c].max_degree += d_max_degree;
            this->vertex[r+1][c].max_degree += d_max_degree;
            this->vertex[r][c].degree += d_degree;
            this->vertex[r+1][c].degree += d_degree;
            break;
        case DOWN:
            this->edge[2*r+2][c].status = status;
            this->vertex[r+1][c].max_degree += d_max_degree;
            this->vertex[r+1][c+1].max_degree += d_max_degree;
            this->vertex[r+1][c].degree += d_degree;
            this->vertex[r+1][c+1].degree += d_degree;
            break;
        case RIGHT:
            this->edge[2*r+1][c+1].status = status;
            this->vertex[r][c+1].max_degree += d_max_degree;
            this->vertex[r+1][c+1].max_degree += d_max_degree;
            this->vertex[r][c+1].degree += d_degree;
            this->vertex[r+1][c+1].degree += d_degree;
            break;
    }
    
}

// set the status of an edge by specifying the position of a vertex and where edge locates
void Puzzle::set_edge_v(int r, int c, int direction, int status){
    assert(PROHIBITED <= status and status <= DECIDED);
    bool out_of_range = r < 0 or this->rows < r 
                     or c < 0 or this->cols < c;
    if(out_of_range) return;

    int d_max_degree = (status == PROHIBITED) ? -1 : 0;
    int d_degree = (status == DECIDED) ? 1 : 0;
    switch(direction){
        case UP:
            if(r == 0 or this->edge[2*r-1][c].status == status) return;
            this->edge[2*r-1][c].status = status;
            this->vertex[r-1][c].max_degree += d_max_degree;
            this->vertex[r-1][c].degree += d_degree;
            break;
        case LEFT:
            if(c == 0 or this->edge[2*r][c-1].status == status) return;
            this->edge[2*r][c-1].status = status;
            this->vertex[r][c-1].max_degree += d_max_degree;
            this->vertex[r][c-1].degree += d_degree;
            break;
        case DOWN:
            if(r == this->rows or this->edge[2*r+1][c].status == status) return;
            this->edge[2*r+1][c].status = status;
            this->vertex[r+1][c].max_degree += d_max_degree;
            this->vertex[r+1][c].degree += d_degree;
            break;
        case RIGHT:
            if(c == this->cols or this->edge[2*r][c].status == status) return;
            this->edge[2*r][c].status = status;
            this->vertex[r][c+1].max_degree += d_max_degree;
            this->vertex[r][c+1].degree += d_degree;
            break;
    }
    this->vertex[r][c].max_degree += d_max_degree;
    this->vertex[r][c].degree += d_degree;
}

