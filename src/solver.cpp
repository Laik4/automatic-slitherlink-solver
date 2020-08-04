#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <cassert>
#include <stack>
#include <utility>

//#define DEBUG 
#define UP    0
#define LEFT  1
#define DOWN  2
#define RIGHT 3

#define PROHIBITED -1
#define PENDING     0
#define DECIDED     1

using namespace std;


class Vertex{
private:
public:
    int max_degree;
    Vertex& operator=(int x){
        assert(0 <= x and x <= 4);
        this->max_degree = x;
        return *this;
    }
    Vertex& operator--(int x){
        this->max_degree = this->max_degree - 1;
        return *this;
    }
    Vertex& operator-=(int x){
        this->max_degree = this->max_degree - x;
        return *this;
    }
    

};

class Edge{
private:
public:
    int status;
    Edge& operator=(int x){
        this->status = x;
        return *this;
    }
    bool operator==(const int x){
        return this->status == x;
    }
    bool operator!=(const int x){
        return !(*this == x);
    }
    operator int() const {
        return status;
    }
    Edge(){
    }
    Edge(int x){
        this->status = x;
    }

};


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


    // get the status of an edge which connects (r1, c1) and (r2, c2)
    Edge get_edge(int r1, int c1, int r2, int c2){
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
    Edge get_edge(int r, int c, int direction){
        bool out_of_range = r < 0 or this->rows < r 
                         or c < 0 or this->cols < c;
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
    Edge get_edge_v(int r, int c, int direction){
        bool out_of_range = r < 0 or this->rows <= r 
                         or c < 0 or this->cols <= c;
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
    void set_edge(int r1, int c1, int r2, int c2, int status){
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
    void set_edge(int r, int c, int direction, int status){
        // direction   [0:up] [1:left] [2:down] [3:left]
        assert(PROHIBITED <= status and status <= DECIDED);
        bool out_of_range = r < 0 or this->rows < r 
                         or c < 0 or this->cols < c;
        if(out_of_range)  return;

        // Update the status of an edge and the max_degree of each vertex
        int d_degree = (status == PROHIBITED) ? -1 : 0;
        switch(direction){
            case UP:
                this->edge[2*r][c].status = status;
                this->vertex[r][c].max_degree += d_degree;
                this->vertex[r][c+1].max_degree += d_degree;
                break;
            case LEFT:
                this->edge[2*r+1][c].status = status;
                this->vertex[r][c].max_degree += d_degree;
                this->vertex[r+1][c].max_degree += d_degree;
                break;
            case DOWN:
                this->edge[2*r+2][c].status = status;
                this->vertex[r+1][c].max_degree += d_degree;
                this->vertex[r+1][c+1].max_degree += d_degree;
                break;
            case RIGHT:
                this->edge[2*r+1][c+1].status = status;
                this->vertex[r][c+1].max_degree += d_degree;
                this->vertex[r+1][c+1].max_degree += d_degree;
                break;
        }
        
    }

    // set the status of an edge by specifying the position of a vertex and where edge locates
    void set_edge_v(int r, int c, int direction, int status){
        assert(PROHIBITED <= status and status <= DECIDED);
        bool out_of_range = r < 0 or this->rows <= r 
                         or c < 0 or this->cols <= c;
        if(out_of_range) return;

        int d_degree = (status == PROHIBITED) ? -1 : 0;
        switch(direction){
            case UP:
                if(r == 0 or this->edge[2*r-1][c].status == status) return;
                this->edge[2*r-1][c].status = status;
                this->vertex[r-1][c].max_degree += d_degree;
                break;
            case LEFT:
                if(c == 0 or this->edge[2*r][c-1].status == status) return;
                this->edge[2*r][c-1].status = status;
                this->vertex[r][c-1].max_degree += d_degree;
                break;
            case DOWN:
                if(r == this->rows or this->edge[2*r+1][c].status == status) return;
                this->edge[2*r+1][c].status = status;
                this->vertex[r+1][c].max_degree += d_degree;
                break;
            case RIGHT:
                if(c == this->cols or this->edge[2*r][c].status == status) return;
                this->edge[2*r][c].status = status;
                this->vertex[r][c+1].max_degree += d_degree;
                break;
        }
        this->vertex[r][c].max_degree += d_degree;
    }
};

class Solver{
private:
    Puzzle puzzle;
    bool satistied(){
        return true;

    }
    void prune_zero(){
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
    int extend_decided(){
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
                cerr << "-----"<<'\n';
                cerr << r << ' ' << c << ": " << decided<< '\n';
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
    int decide_by_prohibited(){
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

    void prune_deadend(){
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

    void show_max_degree(){
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
    void show_status(){
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

public:
    Solver(Puzzle p){
        this->puzzle = p;
    }
    void solve(){
        int satisfiable = 0;
        this->prune_zero();
        for(int i = 0; i < 10; ++i){
            this->prune_deadend();
            satisfiable |= this->decide_by_prohibited();
            satisfiable |= this->extend_decided();
            cerr << i << "=====================" << '\n';
            this->puzzle.show();
            this->show_max_degree();
            this->show_status();

            if(satisfiable < 0){
                cout << "This puzzle is unsatisfiable." << '\n';
                return;
            }
        }
    }

};

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
            int deg = 4;
            if(r == 0 or r == this->rows) --deg;
            if(c == 0 or c == this->cols) --deg;
            vertex[r][c].max_degree = deg;
        }
    }

    // init edge
    for(int r = 0; r <= 2*this->rows; ++r){
        for(int c = 0; c <= this->cols; ++c){
            edge[r][c] = 0;
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

    cout << "┌";
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
        cout << ((c < this->cols-1) ? "┬" : "┐");
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

        cout << ((r < this->rows-1) ? "├" : "└");
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

            if(r < this->rows-1){
                cout << ((c < this->cols-1) ? "┼" : "┤");
            }else{
                cout << ((c < this->cols-1) ? "┴" : "┘");
            }
        }
        cout << '\n';
    }


}


int main(int argc, char **argv){

    Puzzle p;
    p.load(argv[1]);
    Solver s(p);
    s.solve();

    return 0;
    cerr << '\n';
}
