#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <cassert>

using namespace std;


class Vertex{
private:
    int indegree;
    int outdegree;
    int maxedge;
public:

};

class Edge{
private:
// status 0: not decided, 1: line, 2: no line
public:
    int status;
    Edge& operator=(int e){
        this->status = e;
        return *this;
    }

};


class Puzzle{
private:
    vector<vector<Vertex>> vertex;
    vector<vector<Edge>> edge;
public:
    vector<vector<int>> constraint;
    int rows;
    int cols;
    int load(string filename);
    void show(void);
    void set_edge(int r1, int c1, int r2, int c2, int status){
        assert(0 <= status and status <= 2);
        assert(r1 == r2 or c1 == c2);
        if(r1 == r2){
            // -
            this->edge[2*r1][c1] = status;
        }
        if(c1 == c2){
            // |
            this->edge[2*r1+1][c1] = status;
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
                    puzzle.set_edge(r, c, r, c+1, 2);
                    puzzle.set_edge(r, c, r+1, c, 2);
                    puzzle.set_edge(r, c+1, r+1, c+1, 2);
                    puzzle.set_edge(r+1, c, r+1, c+1, 2);
                }
            }
        }

    }

public:

};

int Puzzle::load(string filename){
    ifstream ifs(filename);
    if(!ifs){
        cerr << "Can't open " << filename << ". \n";
        return 1;
    }

    string buf;
    ifs >> rows >> cols;
    getline(ifs, buf);
    this->constraint.resize(rows, vector<int>(cols, -1));
    this->vertex.resize(rows+1, vector<Vertex>(cols+1));
    this->edge.resize(2*rows+1, vector<Edge>(cols+1));

    for(int r = 0; r < rows; ++r){
        for(int c = 0; c < cols; ++c){
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
        cout << ((this->edge[0][c].status == 2) ? "x" : "─");
        if(c < this->cols-1){
            cout << "─┬─";
        }else{
            cout << "─┐\n";
        }
    }

    for(int r = 0; r < this->rows; ++r){
        cout << ((this->edge[2*r+1][0].status == 2) ? "x " : "│ ");
        for(int c = 0; c < this->cols; ++c){
            char num = ' ';
            if(this->constraint[r][c] >= 0) num = this->constraint[r][c]+'0';
            cout << num;
            cout << ((this->edge[2*r+1][c].status == 2) ? " x " : " │ ");
        }
        cout << '\n';

        if(r < this->rows-1){
            cout << "├─";
            for(int c = 0; c < this->cols-1; ++c){
                cout << ((this->edge[2*r][c].status == 2) ? "x" : "─");
                cout << "─┼─";
            }
            cout << "──┤\n";
        }
    }

    cout << "└─";
        for(int c = 0; c < this->cols; ++c){
            cout << ((this->edge[2*this->rows][c].status == 2) ? "x" : "─");
            if(c < this->cols-1){
                cout << "─┴─";
            }else{
                cout << "─┘\n";
            }
        }

}


int main(){

    Puzzle p;
    p.load("samples/easy01.txt");
    p.show();

    return 0;
}
