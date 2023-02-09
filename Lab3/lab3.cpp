#include <iostream>
#include "place.h"
#include "module.h"

using namespace std;

int main(int argc, char *argv[]){

    ifstream in(argv[1]);
    ofstream out(argv[2]);

    Place place;
    place.parser(in);
    place.random_partition();
    if(place.cell_num < 150000){
        place.partition();
    }
    place.group_partition();
    place.initial_row();
    if(place.cell_num < 150000){
        place.abacus();
    }
    else{
        place.tetris();
    }
    place.output(out);

    return 0;
}