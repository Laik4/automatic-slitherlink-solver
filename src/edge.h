#pragma once

class Edge{
private:
public:
    int status;

    Edge(void);
    Edge(int x);
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
};

