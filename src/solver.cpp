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
#define NO_LINE_CHAR "\e[2mx\e[m"
#define DECIDED_COLOR "\e[31"+(c)+"\e[m"
#define UP    0
#define LEFT  1
#define DOWN  2
#define RIGHT 3
#define PROHIBITED -1
#define PENDING 0
#define DECIDED 1

using namespace std;


class Vertex{
private:
public:
    int degree;
    Vertex& operator=(int x){
        assert(0 <= x and x <= 4);
        this->degree = x;
        return *this;
    }
    Vertex& operator--(int x){
        this->degree = this->degree - 1;
        return *this;
    }
    Vertex& operator-=(int x){
        this->degree = this->degree - x;
        return *this;
    }
    

};

class Edge{
private:
// status 0: not decided, 1: line, 2: no line
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
    Edge get_edge(int r1, int c1, int r2, int c2){
        // get the status of an edge which connects (r1, c1) and (r2, c2)
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
    void set_edge(int r1, int c1, int r2, int c2, int status){
        // set the status of an edge which connects (r1, c1) and (r2, c2)
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
#ifdef DEBUG
            cerr << "[+] set_edge: (" << r1 << ", " << c1 << ")-(" << r2 << ", " << c2 << ") [Edge ("<< 2*r1 << ", " << c1 << ")] <= " << status << '\n';
#endif
            this->edge[2*r1][c1] = status;
        }
        if(c1 == c2){
#ifdef DEBUG
            cerr << "[+] set_edge: (" << r1 << ", " << c1 << ")-(" << r2 << ", " << c2 << ") [Edge ("<< 2*r1+1 << ", " << c1 << ")] <= " << status << '\n';
#endif
            this->edge[2*r1+1][c1] = status;
        }

    }

    void set_edge(int r, int c, int direction, int status){
        // direction   [0:up] [1:left] [2:down] [3:left]
        assert(PROHIBITED <= status and status <= DECIDED);
        bool out_of_range = r < 0 or this->rows < r 
                         or c < 0 or this->cols < c;
        if(out_of_range)  return;

        // Update the status of an edge and the degree of each vertex
        switch(direction){
            case UP:
                this->edge[2*r][c].status = status;
                this->vertex[r][c].degree += status;
                this->vertex[r][c+1].degree += status;
                break;
            case LEFT:
                this->edge[2*r+1][c].status = status;
                this->vertex[r][c].degree += status;
                this->vertex[r+1][c].degree += status;
                break;
            case DOWN:
                this->edge[2*r+2][c].status = status;
                this->vertex[r+1][c].degree += status;
                this->vertex[r+1][c+1].degree += status;
                break;
            case RIGHT:
                this->edge[2*r+1][c+1].status = status;
                this->vertex[r][c+1].degree += status;
                this->vertex[r+1][c+1].degree += status;
                break;
        }
        
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
    int determine_by_prohibited(){
        int d[5] = {0, 0, 1, 1};
        for(int r = 0; r < puzzle.rows; ++r){
            for(int c = 0; c < puzzle.cols; ++c){
                // count the number of prohibited edges around (r, c)
                int prohibited_num = 0;
                for(int i = 0; i < 5; ++i){
                    if(puzzle.get_edge(r+d[i], c+d[(i+1)%4], r+d[(i+1)%4], c+d[(i+2)%4]) == PROHIBITED){
                        ++prohibited_num;
                    }
                }
                if(puzzle.constraint[r][c] == 4-prohibited_num){
                    for(int i = 0; i < 5; ++i){
                        if(puzzle.get_edge(r+d[i], c+d[(i+1)%4], r+d[(i+1)%4], c+d[(i+2)%4]) == PENDING){
                            ++puzzle.vertex[r+d[i]][c+d[(i+1)%4]].degree;
                            ++puzzle.vertex[r+d[(i+1)%4]][c+d[(i+2)%4]].degree;
                            puzzle.set_edge(r+d[i], c+d[(i+1)%4], r+d[(i+1)%4], c+d[(i+2)%4], DECIDED);
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
                if(puzzle.vertex[r][c].degree == DECIDED){
                    --puzzle.vertex[r][c].degree;
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
                if(puzzle.vertex[R][C].degree > 0){
                    --puzzle.vertex[R][C].degree;
                    puzzle.set_edge(r, c, R, C, PROHIBITED);
                }
                bool degree_is_1 = puzzle.vertex[R][C].degree == 1;
                if(degree_is_1){
                    q.emplace(R, C);
                }
            }
        }
    }

    void show_degree(){
        for(int r = 0; r <= puzzle.rows; ++r){
            for(int c = 0; c <= puzzle.cols; ++c){
                cerr << puzzle.vertex[r][c].degree;
                if(c < puzzle.cols)  cerr << "───";
            }
            cerr << '\n';
            if(r < puzzle.rows){
                for(int c = 0; c <= puzzle.cols; ++c){
                    cerr << "│   ";
                }
            }
            cerr << '\n';
        }
    }
    void show_status(){
        cout << "┌─";
        for(int c = 0; c < puzzle.cols; ++c){
            cout << (puzzle.edge[0][c].status);
            if(c < puzzle.cols-1){
                cout << "─┬─";
            }else{
                cout << "─┐\n";
            }
        }

        for(int r = 0; r < puzzle.rows; ++r){
            cout << (puzzle.edge[2*r+1][0].status);
            for(int c = 0; c < puzzle.cols; ++c){
                cout << "   ";
                cout << (puzzle.edge[2*r+1][c+1].status);
            }
            cout << '\n';

            if(r < puzzle.rows-1){
                cout << "├─";
                for(int c = 0; c < puzzle.cols; ++c){
                    cout << puzzle.edge[2*r+2][c].status;
                    if(c < puzzle.cols-1){
                        cout << "─┼─";
                    }else{
                        cout << "─┤\n";
                    }
                }
            }
        }

        cout << "└─";
        for(int c = 0; c < puzzle.cols; ++c){
            cout << (puzzle.edge[2*puzzle.rows][c].status);
            if(c < puzzle.cols-1){
                cout << "─┴─";
            }else{
                cout << "─┘\n";
            }
        }

    }

public:
    Solver(Puzzle p){
        this->puzzle = p;
    }
    void solve(){
        this->prune_zero();
        this->prune_deadend();
        this->puzzle.show();
        this->show_degree();
        this->show_status();
        this->determine_by_prohibited();
        this->puzzle.show();
        this->show_degree();
        this->show_status();
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
            vertex[r][c].degree = deg;
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
    cout << "┌─";
    for(int c = 0; c < this->cols; ++c){
        cout << ((this->edge[0][c].status == PROHIBITED) ? NO_LINE_CHAR : "─");
        if(c < this->cols-1){
            cout << "─┬─";
        }else{
            cout << "─┐\n";
        }
    }

    for(int r = 0; r < this->rows; ++r){
        cout << ((this->edge[2*r+1][0].status == PROHIBITED) ? NO_LINE_CHAR : "│");
        for(int c = 0; c < this->cols; ++c){
            string num = "   ";
            if(this->constraint[r][c] >= 0) num[1] = this->constraint[r][c]+'0';
            cout << num;
            cout << ((this->edge[2*r+1][c+1].status == PROHIBITED) ? NO_LINE_CHAR : "│");
        }
        cout << '\n';

        if(r < this->rows-1){
            cout << "├─";
            for(int c = 0; c < this->cols; ++c){
                cout << ((this->edge[2*r+2][c].status == PROHIBITED) ? NO_LINE_CHAR : "─");
                if(c < this->cols-1){
                    cout << "─┼─";
                }else{
                    cout << "─┤\n";
                }
            }
        }
    }

    cout << "└─";
    for(int c = 0; c < this->cols; ++c){
        cout << ((this->edge[2*this->rows][c].status == PROHIBITED) ? NO_LINE_CHAR : "─");
        if(c < this->cols-1){
            cout << "─┴─";
        }else{
            cout << "─┘\n";
        }
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
