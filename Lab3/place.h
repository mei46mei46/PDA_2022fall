#ifndef PLACE_H
#define PLACE_H

#include <vector>
#include <fstream>
#include <list>
#include <map>
#include <iostream>
#include <algorithm>
#include <string>
#include <math.h>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <limits.h>
#include "module.h"

using namespace std;


class Place
{
    friend bool compare_xpos(const Cell* a,const Cell* b);
    
 public:
    Place() : num_net(0){ }
	~Place(){ }
    void parser(ifstream& in);
    void random_partition();
    void group_partition();
    void initial_row();
    void output(ofstream &out);

    void tetris();
    void tetris_placeCell(int flag,int c,int j);

    void abacus();
    void add_cell(int flag,int i, Cluster& c, int r);
    void add_cluster(int s,int t);
    void placeRow(int flag, int r);
    void collapse(int c,int r);


    void graph_construct();
    void set_fs_te();
    void calculate_gain();
    void run_FM();
    void update_gain();
    void move_node();
    void find_max();
    void calculate_factor();
    void calculate_factor0();
    void calculate_factor1();
    void calculate_cutsize();
    void partition();
    double distance(int c,int r);
    int cell_num;
 private:
    int die_width, die_height;
    int ter_num, row_num, row_height;
    vector<Terminal> ter_list;
    vector<Cell> cell_list;

    int group0_num, group1_num;
    vector<int> cell_list_g0;
    vector<int> cell_list_g1;
    map<string, int> name2idex;


    vector<Cell> cell_list_cp;
    //vector< vector<int> > net;
    vector<Net> net;
    vector<Row> row_list;
    vector<Row> row_list_tg1;
    vector<int> weight;
    int num_net;
    
    long long cutsize;
    map<string,Cell*> name2cell;

};

#endif