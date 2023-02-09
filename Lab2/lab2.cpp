#include <iostream>
#include <fstream>
#include <time.h>
#include "b_star.h"
#include "module.h"

using namespace std;

int main(int argc, char *argv[]){
    clock_t end,start;
    start = clock();
    
    double alpha = atof(argv[1]);
    ifstream in_block(argv[2]);
    ifstream in_net(argv[3]);
    ofstream out(argv[4]);

    Floorplanner* FP = new Floorplanner(alpha);
    FP->parser_block(in_block);
    FP->parser_net(in_net);
    FP->initialize();
    FP->SA(start);
    FP->print_rpt(out,start);
    end = clock();
    //cout<<double(end-start)/ CLOCKS_PER_SEC<<"s"<<endl;
    return 0;
}