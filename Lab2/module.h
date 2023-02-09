#ifndef MODULE_H
#define MODULE_H

#include <vector>

using namespace std;

class Block{
 public:
    Block(string str, double w_, double h_) : name(str), w(w_), h(h_) { }
    ~Block() { }
    double w, h;
    string name;

};

class Terminal{
 public:
    Terminal(string str, double _x, double _y) : name(str), x(_x), y(_y) { }
    ~Terminal() { }
    double x, y;
    string name;
};

class Net{
 public:
    Net(int num) : NetDegree(num) { }
    ~Net() { }
    
    int NetDegree;
    vector<string> include_BandT;
};

class Node{
 public:
    Node(string name_="",double w_=0,double h_=0,double x_=0,double y_=0) : name(name_), w(w_), h(h_), x(x_), y(y_), left(NULL), right(NULL), parent(NULL) { }
    ~Node() { }
    double get_center_x(){ return (double)(x + x + w)/2; }
    double get_center_y(){ return (double)(y + y + h)/2; }
    string name;
    double x, y;
    double w, h;
    Node *left, *right, *parent;
};

class contour{
 public:
    contour(double x_=0,double x1_=0, double y_=0) : x1(x_),x2(x1_),y(y_){ }
    ~contour() { }
    double x1, x2, y;
};

#endif 