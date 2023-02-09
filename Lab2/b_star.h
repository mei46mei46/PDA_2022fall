#ifndef B_STAR_H
#define B_STAR_H

#include <vector>
#include <fstream>
#include <list>
#include <map>
#include <iostream>
#include <string>
#include <math.h>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include "module.h"

using namespace std;


class Floorplanner
{
 public:
    Floorplanner(double num){ alpha = num; }
	~Floorplanner(){}
    void parser_block(ifstream& in_block);
    void parser_net(ifstream& in_net);
    void initialize();
    void SA(clock_t start);
    void print_rpt(ofstream& out,clock_t start);

 private:
    bool is_legal;
    double alpha;
    double min_cost;
    double max_x,max_y;
    int outline_width,outline_height;
    int NumBlocks, NumTerminals, NumNets;
    int HPWL_;
    vector<Block*> block_set;
    vector<Terminal*> terminal_set;
    vector<Net*> net_set;
    vector<Node*> node_set;
    list<contour> contour_line;
    Node* root;
    map<string, Node*> search_node;
    map<string, Block*> search_block;
    map<string, Terminal*> search_terminal;
    void set_initial_tree();
    void set_coordinate(Node* Node);
    void check_legal();
    void after_movement();
    void rotate_blk(int blk_num);
    void swap_blk(int blk1,int blk2);
    void del_and_ins_blk(int blk1,int blk2);
    void recover(int del,int ins,int prev_parent,int prev_child,Node* prev_parentBlk,Node* prev_childBlk);
    void preorder(Node* node);
    int HPWL();
    int wirelength(Net* net);
    double update_contour(Node* Node);
    double cal_area_cost();
    double cal_cost();
};

#endif 