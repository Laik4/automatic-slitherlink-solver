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


class Puzzle{
private:
    vector<vector<int>> constraint;
    vector<vector<Vertex>> vertex;
    int rows;
    int cols;
public:
    int load(string filename);
    void show(void);

};

class Solver{
private:
    bool satistied(Puzzle g){
        return true;

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

    for(int h = 0; h < rows; ++h){
        for(int w = 0; w < cols; ++w){
            char buf;
            ifs >> buf;
            if(buf == 'x'){
                continue;
            }else{
                assert('0' <= buf and buf <= '3');
                this->constraint[h][w] = buf-'0'; 
            }
        }
    }

    return 0;
}

void Puzzle::show(void){
    cout << "┌─";
    for(int i = 0; i < this->cols-1; ++i){
        cout << "──┬─";
    }
    cout << "──┐\n";

    for(int h = 0; h < this->rows; ++h){
        cout << "│ ";
        for(int w = 0; w < this->cols; ++w){
            char num = ' ';
            if(this->constraint[h][w] >= 0) num = this->constraint[h][w]+'0';
            cout << num << " │ ";
        }
        cout << '\n';

        if(h < this->rows-1){
            cout << "├─";
            for(int i = 0; i < this->cols-1; ++i){
                cout << "──┼─";
            }
            cout << "──┤";
        }else{
            cout << "└─";
            for(int i = 0; i < this->cols-1; ++i){
                cout << "──┴─";
            }
            cout << "──┘\n";
        }
        cout << '\n';
    }
}


int main(){

    Puzzle p;
    p.load("samples/easy01.txt");
    p.show();

    return 0;
}
