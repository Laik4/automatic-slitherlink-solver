#pragma once

#include <cassert>

class Vertex{
private:
public:
    int max_degree;
    int degree;
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

