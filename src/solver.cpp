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


class Grid{
private:
    vector<vector<int>> constraint;
    vector<vector<Vertex>> vertex; 
    int height;
    int width;
public:
    int load(string filename);
    void show(void);

};

class Solver{
private:
    bool satistied(Grid g){
        return true;

    }

public:

};
int Grid::load(string filename){
    ifstream ifs(filename);
    if(!ifs){
        cerr << "Can't open " << filename << ". \n";
        return 1;
    }

    string buf;
    ifs >> height >> width;
    getline(ifs, buf);
    this->constraint.resize(height, vector<int>(width, -1));

    for(int h = 0; h < height; ++h){
        for(int w = 0; w < width; ++w){
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

void Grid::show(void){
    cout << "┌─";
    for(int i = 0; i < this->width-1; ++i){
        cout << "──┬─";
    }
    cout << "──┐\n";

    for(int h = 0; h < this->height; ++h){
        cout << "│ ";
        for(int w = 0; w < this->width; ++w){
            char num = ' ';
            if(this->constraint[h][w] >= 0) num = this->constraint[h][w]+'0';
            cout << num << " │ ";
        }
        cout << '\n';

        if(h < this->height-1){
            cout << "├─";
            for(int i = 0; i < this->width-1; ++i){
                cout << "──┼─";
            }
            cout << "──┤";
        }else{
            cout << "└─";
            for(int i = 0; i < this->width-1; ++i){
                cout << "──┴─";
            }
            cout << "──┘\n";
        }
        cout << '\n';
    }
}


int main(){

    Grid g;
    g.load("samples/easy01.txt");
    g.show();

    return 0;
}
