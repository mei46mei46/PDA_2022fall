#ifndef MODULE_H
#define MODULE_H

#include <vector>

using namespace std;

class Net_pair;

class Cell{
 public:
    Cell(string str, int _lx, int _ly,int _hx,int _hy,int _idx) : name(str), lx(_lx), ly(_ly), hx(_hx), hy(_hy), idx(_idx), gain(0), status(0), fs(0), te(0), o_lx(_lx), o_ly(_ly), width(_hx-_lx){ }
    ~Cell() { }
    int lx, ly, hx, hy, width, o_lx, o_ly, idx;
    string name;
    int gain, status, group, fs, te;
    vector<Net_pair> net_pair;
};

class Terminal{
 public:
    Terminal(string str, int _lx, int _ly,int _hx,int _hy) : name(str), lx(_lx), ly(_ly), hx(_hx), hy(_hy) { }
    ~Terminal() { }
    int lx, ly, hx, hy;
    string name;
};

class Row{
 public:
    Row(int lx_, int hx_, int y_) : lx(lx_), hx(hx_),y(y_), totalWidth_g0(0), totalWidth_g1(0) { }
    ~Row() { }
    int lx, y, hx, idx;
    vector<int> row_cell_g0;
    vector<int> row_cell_g1;
    int totalWidth_g0, totalWidth_g1;
};

class Net_pair{
 public:
    Net_pair(int _weight,int _cell): cell(_cell), weight(_weight) { };
    ~Net_pair() { };
    int cell;
    int weight;
};

class Net{
 public:
    Net(int _weight,int _cell1,int _cell2): weight(_weight), cell1(_cell1), cell2(_cell2) { };
    ~Net() { };
    int cell1, cell2;
    int weight;
};

struct Cluster{
    Cluster() {
        ec = 0;
        wc = 0;
        qc = 0;
    }
    int ec; //weight
    int wc; //width
    int qc; //quadratic
    int pos_x;
    int firstCell,lastCell;
    vector<int> c_cell_list;
};

#endif