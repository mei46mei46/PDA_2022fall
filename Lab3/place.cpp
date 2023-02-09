
#include "place.h"
#include "module.h"

using namespace std;

double factor0,factor1;
bool flag_imp;
int target;
int best_cutsize;

vector<Cluster> c_list;

bool legal;
bool final_;

bool compare_xpos(const Cell a,const Cell b){
    if(a.lx == b.lx){
        return (a.ly)<(b.ly);
    }
    else{
        return (a.lx)<(b.lx);
    }
}
bool compare_ypos(const Terminal a,const Terminal b){
    if(a.ly == b.ly){
        return (a.lx)<(b.lx);
    }
    else{
        return (a.ly)<(b.ly);
    }
}
bool compare_ypos_row(const Row a,const Row b){
    if(a.y == b.y){
        return (a.lx)<(b.lx);
    }
    else{
        return (a.y)<(b.y);
    }
}
bool compare_idx(const Cell a,const Cell b){
    return (a.idx)<(b.idx);
}
bool compare_group(const int a,const int b){
    return a < b;
}

/*===============================================================================*/
/*                                   < parser >                                  */
/*===============================================================================*/

void Place::parser(ifstream& in){
    string temp;
    int int1, int2, int3, int4;
    in >> temp >> int1 >> int2; //DieSize 30 30
    die_width = int1;
    die_height = int2;
    in >> temp >> int1 >> int2; //DieRows 10 3 height, number
    row_height = int1;
    row_num = int2;
    in >> temp >> int1; //Terminal 2
    ter_num = int1;
    for(int i=0; i<ter_num; i++){
        in >> temp >> int1 >> int2 >> int3 >> int4; //T0 0 0 2 2
        Terminal temp_ter(temp, int1, int2, int1+int3, int2+int4);
        ter_list.push_back(temp_ter);
    }
    in >> temp >> int1; //NumCell 4
    cell_num = int1;
    for(int i=0; i<cell_num; i++){
        in >> temp >> int1 >> int2 >> int3 >> int4; //C0 0 3 10 10
        int index;
        index = stoi(temp.substr(1));
        Cell temp_cell(temp, int1, int2, int1+int3, int2+int4, index);
        cell_list.push_back(temp_cell);
    }
    /*cell_list_cp.assign(cell_list.begin(), cell_list.end());
    cell_list.insert(cell_list.end(), cell_list.begin(), cell_list.end());
    cell_list.insert(cell_list.end(), cell_list.begin(), cell_list.end());
    cell_list.insert(cell_list.end(), cell_list_cp.begin(), cell_list_cp.end());
    cell_num = cell_list.size();*/
    //cell_list.insert(cell_list.end(), cell_list_cp.begin(), cell_list_cp.end());
    //cell_list.insert(cell_list.end(), cell_list.begin(), cell_list.end());
    //cell_list.insert(cell_list.end(), cell_list.begin(), cell_list.end());
    sort(cell_list.begin(), cell_list.end(), compare_xpos);
    /*for(int i=0; i<cell_list.size(); i++){
        name2idex.insert(pair<string, int>(cell_list[i].name, i));
    }*/
    //check
    /*cout<<"width,height: "<<die_width<<", "<<die_height<<endl;
    cout<<"row_num,row_height: "<<row_num<<", "<<row_height<<endl;
    cout<<"ter_size: "<<ter_list.size()<<endl;
    cout<<"cell_size: "<<cell_list.size()<<endl;*/
    /*for(int i=0;i<cell_num;i++){
        cout<<cell_list[i].name<<" "<<cell_list[i].idx<<endl;
    }*/
}

/*===============================================================================*/
/*                                  < partition >                                */
/*===============================================================================*/

void Place::random_partition(){
    group0_num = 0;
    group1_num = 0;
	int y;
	y = cell_num/2;
    for(int i=0; i<cell_num; i++){ //check
        cell_list[i].group = i%2;
        if(i%2 == 0){
            group0_num++;
        }
    }
    group1_num = cell_num - group0_num;
    /*cout<<"g1: "<<group1_num<<endl;
    cout<<"g0: "<<group0_num<<endl;*/
}

void Place::group_partition(){
    for(int i=0; i<cell_num; i++){ //check
        if(cell_list[i].group == 0){
            cell_list_g0.push_back(i);
        }
        else{
            cell_list_g1.push_back(i);
        }
    }
}

/*===============================================================================*/
/*                                 < initialize >                                */
/*===============================================================================*/

void Place::initial_row(){
    sort(ter_list.begin(), ter_list.end(), compare_ypos);
    int y = 0;
    for(int i=0; i<row_num; i++){
        Row row (0, die_width, y);
        row_list.push_back(row);
        y += row_height;
    }

    for(int i=0; i<ter_num; i++){
        Terminal ter = ter_list[i];
        for(int j=0; j<row_list.size(); j++){
            /*if(row_list[j].y >= ter.hy){
                sort(row_list.begin(), row_list.end(), compare_ypos_row);
                break;
            }*/
            if(((row_list[j].y + row_height) <= ter.ly)||(row_list[j].y >= ter.hy)){
                continue;
            }
            else if((row_list[j].lx >= ter.hx)||(row_list[j].hx <= ter.lx)){
                continue;
            }

            if((row_list[j].lx < ter.lx) && (row_list[j].hx > ter.hx)){
                Row row(ter.hx, row_list[j].hx, row_list[j].y);
                row_list[j].hx = ter.lx;
                row_list.push_back(row);
            }
            else if((row_list[j].lx >= ter.lx) && (row_list[j].lx < ter.hx) && (row_list[j].hx > ter.hx)){
                row_list[j].lx = ter.hx;
            }
            else if((row_list[j].lx < ter.lx) && (row_list[j].hx > ter.lx) && (row_list[j].hx <= ter.hx)){
                row_list[j].hx = ter.lx;
            }
            else if((row_list[j].lx >= ter.lx) && (row_list[j].hx <= ter.hx)){
                row_list.erase (row_list.begin()+j);
                j--;
            }
            else{
                cout<<"warning!!"<<endl;
                cout<<i<<endl;
                cout<<ter.name<<", "<<ter.hy<<", "<<row_list[i].lx<<", "<<row_list[i].hx<<", "<<row_list[i].y<<endl;
            }
        }
        sort(row_list.begin(), row_list.end(), compare_ypos_row);
    }
    sort(row_list.begin(), row_list.end(), compare_ypos_row);
    /*for(int i=0; i<row_list.size(); i++){
        cout<<row_list[i].lx<<", "<<row_list[i].hx<<", "<<row_list[i].y<<endl;
    }*/

}

/*===============================================================================*/
/*                                   < output >                                  */
/*===============================================================================*/

void Place::output(ofstream &out){
    sort(cell_list.begin(), cell_list.end(), compare_idx);
    for(int i=0;i<cell_num;i++){
        out<<cell_list[i].name<<" "<<cell_list[i].lx<<" "<<cell_list[i].ly<<" "<<cell_list[i].group<<endl;
    }
}

/*===============================================================================*/
/*                                   < tetris >                                  */
/*===============================================================================*/

double Place::distance(int c,int r){
    int x1 = cell_list[c].lx;
    int y1 = cell_list[c].ly;
    int x2 = row_list[r].lx;
    int y2 = row_list[r].y;

    int temp = x1-x2;
    int temp2 = y1-y2;
    return temp*temp + temp2*temp2;
}

void Place::tetris(){
    row_list_tg1.assign(row_list.begin(),row_list.end());
    vector<int> cell_not_done;
    for(int i=0; i<cell_list_g0.size(); i++){
        int start_row;
        bool if_place=0;
        int cell_int = cell_list_g0[i];
        for(int j=0; j<row_list.size();j++){
            if((row_list[j].y + row_height) < cell_list[cell_int].ly){
                continue;
            }
            else if((row_list[j].y<= cell_list[cell_int].ly) && ((row_list[j].y + row_height) > cell_list[cell_int].ly)){
                start_row = j;
                break;
            }
            else if(row_list[j].y >= cell_list[cell_int].hy){
                start_row = j;
                break;
            }
        }
        int row_best = -1;
        int cost_min = INT_MAX;
        for(int j=start_row; j<row_list.size(); j++){
            int cost;
            if((row_list[j].hx - row_list[j].lx >= cell_list[cell_int].width) && (row_list[j].y == row_list[start_row].y)){
                cost = distance(cell_int, j);
                if(cost < cost_min){
                    row_best = j;
                    cost_min = cost;
                }
                if_place = 1;
                if(j == row_list.size()-1){
                    tetris_placeCell(0,cell_int,row_best);
                    break;
                }
            }
            else if(if_place){
                tetris_placeCell(0,cell_int,row_best);
                break;
            }
            else{
                if(row_list[j].hx - row_list[j].lx >= cell_list[cell_int].width){
                    tetris_placeCell(0,cell_int,j);
                    if_place = 1;
                    break;
                }
            }
        }
        if(!if_place){
            for(int j=start_row-1; j>=0; j--){
                if(row_list[j].hx - row_list[j].lx >= cell_list[cell_int].width){
                    tetris_placeCell(0,cell_int,j);
                    if_place = 1;
                    break;
                }
            }
        }
        if(!if_place){
            cell_list[cell_int].group = 1;
            cell_list_g1.push_back(cell_int);
            cell_list_g0.erase(cell_list_g0.begin()+i);
            i--;
            sort(cell_list_g1.begin(),cell_list_g1.end(),compare_group);
        }
    }
    for(int i=0; i<cell_list_g1.size(); i++){
        int start_row;
        bool if_place=0;
        int cell_int = cell_list_g1[i];
        for(int j=0; j<row_list_tg1.size();j++){
            if((row_list_tg1[j].y + row_height) < cell_list[cell_int].ly){
                continue;
            }
            else if((row_list_tg1[j].y<= cell_list[cell_int].ly) && ((row_list_tg1[j].y + row_height) > cell_list[cell_int].ly)){
                start_row = j;
                break;
            }
            else if(row_list_tg1[j].y >= cell_list[cell_int].hy){
                start_row = j;
                break;
            }
        }
        int row_best = -1;
        int cost_min = INT_MAX;
        for(int j=start_row; j<row_list_tg1.size(); j++){
            int cost;
            if((row_list_tg1[j].hx - row_list_tg1[j].lx >= cell_list[cell_int].width) && (row_list_tg1[j].y == row_list_tg1[start_row].y)){
                cost = distance(cell_int, j);
                if(cost < cost_min){
                    row_best = j;
                    cost_min = cost;
                }
                if_place = 1;
                if(j == row_list_tg1.size()-1){
                    tetris_placeCell(1,cell_int,row_best);
                    break;
                }
            }
            else if(if_place){
                tetris_placeCell(1,cell_int,row_best);
                break;
            }
            else{
                if(row_list_tg1[j].hx - row_list_tg1[j].lx >= cell_list[cell_int].width){
                    tetris_placeCell(1,cell_int,j);
                    if_place = 1;
                    break;
                }
            }
        }
        if(!if_place){
            for(int j=start_row-1; j>=0; j--){
                if(row_list_tg1[j].hx - row_list_tg1[j].lx >= cell_list[cell_int].width){
                    tetris_placeCell(1,cell_int,j);
                    if_place = 1;
                    break;
                }
            }
        }
        if(!if_place){
            cell_list[cell_int].group = 0;
            cell_not_done.push_back(cell_int);
            cell_list_g1.erase(cell_list_g1.begin()+i);
            i--;
        }
    }//cout<<"cell_not_done.size(): "<<cell_not_done.size()<<endl;
    for(int i=0; i<cell_not_done.size();i++){
        int cell_int = cell_not_done[i];
        for(int j=0; j<row_list.size(); j++){
            if(row_list[j].hx - row_list[j].lx >= cell_list[cell_int].width){
                tetris_placeCell(0,cell_int,j);
                break;
            }
        }
    }
}

void Place::tetris_placeCell(int flag,int c,int j){
    if(!flag){
        cell_list[c].lx = row_list[j].lx;
        cell_list[c].ly = row_list[j].y;
        row_list[j].lx = row_list[j].lx + cell_list[c].width;
    }
    else{
        cell_list[c].lx = row_list_tg1[j].lx;
        cell_list[c].ly = row_list_tg1[j].y;
        row_list_tg1[j].lx = row_list_tg1[j].lx + cell_list[c].width;
    }
}

/*===============================================================================*/
/*                                   < abacus >                                  */
/*===============================================================================*/

void Place::add_cell(int flag,int i, Cluster& c, int r){
    //i = the index in row_cell_g
    if(!flag){
        int index = row_list[r].row_cell_g0[i];
        c.lastCell = i;
        c.ec ++;
        c.qc += (cell_list[index].o_lx-c.wc);
        c.wc += cell_list[index].width;
    }
    else{
        int index = row_list[r].row_cell_g1[i];
        c.lastCell = i;
        c.ec ++;
        c.qc += (cell_list[index].o_lx-c.wc);
        c.wc += cell_list[index].width;
    }
}

void Place::add_cluster(int s,int t){
    //add cluster t after cluster s
    c_list[s].lastCell = c_list[t].lastCell;
    c_list[s].ec += c_list[t].ec;   
    c_list[s].qc += c_list[t].qc-(c_list[t].ec * c_list[s].wc);
    c_list[s].wc += c_list[t].wc;
}

void Place::collapse(int c,int r){
    //cout<<c_list[c].qc<<", "<<c_list[c].ec<<endl;
    c_list[c].pos_x = c_list[c].qc / c_list[c].ec;
    if (c_list[c].pos_x < row_list[r].lx) {
        c_list[c].pos_x = row_list[r].lx;
    }
    if (c_list[c].pos_x + c_list[c].wc > row_list[r].hx) {
        c_list[c].pos_x = row_list[r].hx - c_list[c].wc;
    }
    if(c_list.size()==1){
        return;
    }
    if (c_list[c-1].pos_x + c_list[c-1].wc >= c_list[c].pos_x) {
            add_cluster(c-1,c);
            c_list.pop_back();
            int num = c_list.size()-1;
            collapse(num,r);
    }  
}

void Place::placeRow(int flag,int r){
    //r : row index
    if(!flag){
        legal = 1;
        if(!final_){
            int index_last = row_list[r].row_cell_g0.back();
            if ((row_list[r].totalWidth_g0 + cell_list[index_last].width) > (row_list[r].hx - row_list[r].lx)){//cout<<"legal"<<endl;
                legal = 0;
                return;
            }
        }
        int size = row_list[r].row_cell_g0.size();
        for(int i=0; i<size; i++){
            int index = row_list[r].row_cell_g0[i];
            if(c_list.empty()){
                Cluster new_c;
                new_c.pos_x = cell_list[index].o_lx;
                new_c.firstCell = i;
                c_list.push_back(new_c);
                int c_index = c_list.size()-1;
                add_cell(0, i, c_list.back(), r);
                collapse(c_index,r);
            }
            else{
                Cluster c_last = c_list.back();
                if((c_last.pos_x+c_last.wc) < cell_list[index].o_lx){
                    Cluster new_c;
                    new_c.pos_x = cell_list[index].o_lx;
                    new_c.firstCell = i;
                    c_list.push_back(new_c);
                    int c_index = c_list.size()-1;
                    add_cell(0, i, c_list.back(), r);
                    collapse(c_index,r);
                }
                else{
                    int c_index = c_list.size()-1;
                    add_cell(0, i, c_list.back(), r);
                    collapse(c_index,r);
                }
            }
        }
        for(int i=0;i<c_list.size();i++){
            int x= c_list[i].pos_x;
            for(int j=c_list[i].firstCell; j<=c_list[i].lastCell; j++){
                cell_list[row_list[r].row_cell_g0[j]].lx = x;
                cell_list[row_list[r].row_cell_g0[j]].ly = row_list[r].y;
                x = x + cell_list[row_list[r].row_cell_g0[j]].width;
            }
        }
    }
    else{
        legal = 1;
        if(!final_){
            int index_last = row_list[r].row_cell_g1.back();
            if ((row_list[r].totalWidth_g1 + cell_list[index_last].width) > (row_list[r].hx - row_list[r].lx)){//cout<<"legal"<<endl;
                legal = 0;
                return;
            }
        }
        int size = row_list[r].row_cell_g1.size();
        for(int i=0; i<size; i++){
            int index = row_list[r].row_cell_g1[i];
            if(c_list.empty()){
                Cluster new_c;
                new_c.pos_x = cell_list[index].o_lx;
                new_c.firstCell = i;
                c_list.push_back(new_c);
                int c_index = c_list.size()-1;
                add_cell(1, i, c_list.back(), r);
                collapse(c_index,r);
            }
            else{
                Cluster c_last = c_list.back();
                if((c_last.pos_x+c_last.wc) < cell_list[index].o_lx){
                    Cluster new_c;
                    new_c.pos_x = cell_list[index].o_lx;
                    new_c.firstCell = i;
                    c_list.push_back(new_c);
                    int c_index = c_list.size()-1;
                    add_cell(1, i, c_list.back(), r);
                    collapse(c_index,r);
                }
                else{
                    int c_index = c_list.size()-1;
                    add_cell(1, i, c_list.back(), r);
                    collapse(c_index,r);
                }
            }
        }
        for(int i=0;i<c_list.size();i++){
            int x= c_list[i].pos_x;
            for(int j=c_list[i].firstCell; j<=c_list[i].lastCell; j++){
                cell_list[row_list[r].row_cell_g1[j]].lx = x;
                cell_list[row_list[r].row_cell_g1[j]].ly = row_list[r].y;
                x = x + cell_list[row_list[r].row_cell_g1[j]].width;
            }
        }
    }
    c_list.clear();
}


void Place::abacus(){
    final_ = 0;
    vector<int> cell_not_done;
    for(int i=0; i<cell_list_g0.size(); i++){
        int start_row, cost, group;
        int cost_best = INT_MAX;
        int cell_int = cell_list_g0[i];
        int row_best = -1;

        for(int j=0; j<row_list.size();j++){
            if((row_list[j].y + row_height) < cell_list[cell_int].ly){
                continue;
            }
            else if((row_list[j].y<= cell_list[cell_int].ly) && ((row_list[j].y + row_height) > cell_list[cell_int].ly)){
                start_row = j;
                break;
            }
            else if(row_list[j].y >= cell_list[cell_int].hy){
                start_row = j;
                break;
            }
        }

        for(int j=start_row; j<row_list.size();j++){
            //early break
            int y_ = row_list[j].y - cell_list[cell_int].ly;
            if((y_*y_) > cost_best){//cout<<"break"<<endl;
                break;
            }

            row_list[j].row_cell_g0.push_back(cell_int);
            placeRow(0,j);

            if(legal == 1){
                int offset_x, offset_y;
                offset_x = cell_list[cell_int].lx - cell_list[cell_int].o_lx;
                offset_y = cell_list[cell_int].ly - cell_list[cell_int].o_ly;
                cost = offset_x*offset_x + offset_y*offset_y;
            }
            else{
                cost = INT_MAX;
            }
            if(cost < cost_best){
                cost_best = cost;
                row_best = j;
            }
            row_list[j].row_cell_g0.pop_back();
        }

        for(int j=start_row-1; j>=0; j--){
            int y_ = row_list[j].y - cell_list[cell_int].ly;
            if((y_*y_) > cost_best){//cout<<"break"<<endl;
                break;
            }
            
            row_list[j].row_cell_g0.push_back(cell_int);
            placeRow(0,j);

            if(legal == 1){
                int offset_x,offset_y;
                offset_x = cell_list[cell_int].lx - cell_list[cell_int].o_lx;
                offset_y = cell_list[cell_int].ly - cell_list[cell_int].o_ly;
                cost = offset_x*offset_x + offset_y*offset_y;
            }
            else{
                cost = INT_MAX;
            }
            if(cost < cost_best){
                cost_best = cost;
                row_best = j;
            }
            row_list[j].row_cell_g0.pop_back();
        }
        row_list[row_best].row_cell_g0.push_back(cell_int);
        row_list[row_best].totalWidth_g0 += cell_list[cell_int].width;
    }
    for(int i=0; i<cell_list_g1.size(); i++){
        int start_row, cost, group;
        int cost_best = INT_MAX;
        int cell_int = cell_list_g1[i];
        int row_best = -1;

        for(int j=0; j<row_list.size();j++){
            if((row_list[j].y + row_height) < cell_list[cell_int].ly){
                continue;
            }
            else if((row_list[j].y<= cell_list[cell_int].ly) && ((row_list[j].y + row_height) > cell_list[cell_int].ly)){
                start_row = j;
                break;
            }
            else if(row_list[j].y >= cell_list[cell_int].hy){
                start_row = j;
                break;
            }
        }
        for(int j=start_row; j<row_list.size();j++){
            //early break
            int y_ = row_list[j].y - cell_list[cell_int].ly;
            if((y_*y_) > cost_best){
                break;
            }
        
            row_list[j].row_cell_g1.push_back(cell_int);
            placeRow(1,j);

            if(legal == 1){
                int offset_x, offset_y;
                offset_x = cell_list[cell_int].lx - cell_list[cell_int].o_lx;
                offset_y = cell_list[cell_int].ly - cell_list[cell_int].o_ly;
                cost = offset_x*offset_x + offset_y*offset_y;
            }
            else{
                cost = INT_MAX;
            }
            if(cost < cost_best){
                cost_best = cost;
                row_best = j;
            }
            row_list[j].row_cell_g1.pop_back();
        }

        for(int j=start_row-1; j>=0; j--){
            int y_ = row_list[j].y - cell_list[cell_int].ly;
            if((y_*y_) > cost_best){
                break;
            }
            
            row_list[j].row_cell_g1.push_back(cell_int);
            placeRow(1,j);

            if(legal == 1){
                int offset_x,offset_y;
                offset_x = cell_list[cell_int].lx - cell_list[cell_int].o_lx;
                offset_y = cell_list[cell_int].ly - cell_list[cell_int].o_ly;
                cost = offset_x*offset_x + offset_y*offset_y;
            }
            else{
                cost = INT_MAX;
            }
            if(cost < cost_best){
                cost_best = cost;
                row_best = j;
            }
            row_list[j].row_cell_g1.pop_back();
        }
        row_list[row_best].row_cell_g1.push_back(cell_int);
        row_list[row_best].totalWidth_g1 += cell_list[cell_int].width;
    }
    final_ = 1;
    for(int i=0;i<row_list.size();i++){
        placeRow(0,i);
        placeRow(1,i);
    }
}

/*===============================================================================*/
/*                                 < partition >                                 */
/*===============================================================================*/

void Place::graph_construct(){
    for(int i=0; i<cell_list.size(); i++){
        for(int j=i+1;j<cell_list.size(); j++){
            int weight = 0;
            if(cell_list[j].lx >= cell_list[i].hx){
                break;
            }
            else if(cell_list[j].ly > cell_list[i].ly && cell_list[j].ly < cell_list[i].hy && cell_list[j].hy > cell_list[i].hy){
                if(cell_list[j].hx <= cell_list[i].hx){
                    weight = (cell_list[j].hx - cell_list[j].lx) * (cell_list[i].hy - cell_list[j].ly);
                }
                else if(cell_list[j].hx > cell_list[i].hx){
                    weight = (cell_list[i].hx - cell_list[j].lx) * (cell_list[i].hy - cell_list[j].ly);
                }
            }
            else if(cell_list[j].hy > cell_list[i].ly && cell_list[j].hy < cell_list[i].hy && cell_list[j].ly < cell_list[i].ly){
                if(cell_list[j].hx <= cell_list[i].hx){
                    weight = (cell_list[j].hx - cell_list[j].lx) * (cell_list[j].hy - cell_list[i].ly);
                }
                else if(cell_list[j].hx > cell_list[i].hx){
                    weight = (cell_list[i].hx - cell_list[j].lx) * (cell_list[j].hy - cell_list[i].ly);
                }
            }
            else if(cell_list[j].ly > cell_list[i].ly && cell_list[j].hy < cell_list[i].hy){
                if(cell_list[j].hx <= cell_list[i].hx){
                    weight = (cell_list[j].hx - cell_list[j].lx) * (cell_list[j].hy - cell_list[j].ly);
                }
                else if(cell_list[j].hx > cell_list[i].hx){
                    weight = (cell_list[i].hx - cell_list[j].lx) * (cell_list[j].hy - cell_list[j].ly);
                }
            }
            if(weight!=0){
                Net_pair net_pair1(weight, j);
                Net_pair net_pair2(weight, i);
                cell_list[i].net_pair.push_back(net_pair1);
                cell_list[j].net_pair.push_back(net_pair2);
                Net temp_net(weight, i, j);
                net.push_back(temp_net);
                num_net++;
            }
        }
    }
    //cout<<"num_net: "<<num_net<<endl;
}

void Place::set_fs_te(){
    cutsize = 0;
    for(int i=0; i<num_net; i++){
        Cell *cell1 = &cell_list[net[i].cell1];
        Cell *cell2 = &cell_list[net[i].cell2];
        int weight = net[i].weight;
        if(cell1->group != cell2->group){
            cutsize += weight;
            cell1->fs += weight;
            cell2->fs += weight;
        }
        else{
            cell1->te += weight;
            cell2->te += weight;
        }
    }
    /*cout<<"cutsize: "<<cutsize<<endl;
    cout<<"g1: "<<group1_num<<endl;
    cout<<"g0: "<<group0_num<<endl;*/
    /*for(int i=0;i<cell_num;i++){
        cout<<cell_list[i].name<<", "<<cell_list[i].fs<<", "<<cell_list[i].te<<endl;
    }*/
}

void Place::calculate_cutsize(){
    cutsize = 0;
    for(int i=0; i<num_net; i++){
        Cell *cell1 = &cell_list[net[i].cell1];
        Cell *cell2 = &cell_list[net[i].cell2];
        int weight = net[i].weight;
        if(cell1->group != cell2->group){
            cutsize += weight;
        }
    }
    /*cout<<"cutsize_after: "<<cutsize<<endl;
    cout<<"g1: "<<group1_num<<endl;
    cout<<"g0: "<<group0_num<<endl;*/
    calculate_factor();
}

void Place::calculate_gain(){
    for(int i=0; i<cell_num; i++){
        cell_list[i].gain = cell_list[i].te - cell_list[i].fs;
    }
}

void Place::calculate_factor0(){
    factor0 = (double) (group0_num - 1) / (group0_num + group1_num);
    factor1 = (double) (group1_num + 1) / (group0_num + group1_num);
}
void Place::calculate_factor1(){
    factor0 = (double) (group0_num + 1) / (group0_num + group1_num);
    factor1 = (double) (group1_num - 1) / (group0_num + group1_num);
}
void Place::calculate_factor(){
    factor0= (double) group0_num / (group0_num+group1_num);
    factor1=(double) group1_num/(group0_num+group1_num);
    //cout<<"factor: "<<factor0<<", "<<factor1<<endl;
}

void Place::find_max(){
    int max_g0 = -1, max_g1 = -1;
    target = -1;
    int max_gain_g0, max_gain_g1;
    max_gain_g0 = INT_MIN;
    max_gain_g1 = INT_MIN;
    for(int i=0; i<cell_num; i++){
        if(cell_list[i].status == 0){
            if(cell_list[i].group==0){
                if(cell_list[i].gain > max_gain_g0){
                    max_g0 = i;
                    max_gain_g0 = cell_list[i].gain;
                }
            }
            else{
                if(cell_list[i].gain > max_gain_g1){
                    max_g1 = i;
                    max_gain_g1 = cell_list[i].gain;
                }
            }
        }
    }
    if((max_gain_g0 <= 0) && (max_gain_g1 <= 0)){
        flag_imp=1;
    }
    else if((max_g0 == -1)&&(max_g1 == -1)){
        flag_imp=1;
    }
    else if(max_gain_g0 > max_gain_g1){
        calculate_factor0();
        if(factor0 < 0.45 || factor1 > 0.55){
            if((max_g1 == -1)||(max_gain_g1 <= 0)){
                flag_imp=1;
            }
            else{
                target = max_g1;
            }//move max_g1
        }
        else{
            target = max_g0;//move max_g0
        }
    }
    else if(max_gain_g0 < max_gain_g1){
        calculate_factor1();
        if(factor0 > 0.55 || factor1 < 0.45){
            if((max_g0 == -1)||(max_gain_g0 <= 0)){//cout<<"case4"<<endl;
                flag_imp=1;
            }
            else{
                target = max_g0;
            }//move max_g0
        }
        else{
            target = max_g1;//move max_g1
        }
    }
    else{
        calculate_factor0();
        if(factor0 < 0.45 || factor1 > 0.55){
            target = max_g1;
            //move max_g1
        }
        else{
            target = max_g0;//move max_g0
        }
    }
}

void Place::move_node(){
    Cell *cell = &cell_list[target];
    if(cell->group == 0){
        cell->group = 1;
        group0_num--;
        group1_num++;
    }
    else{
        cell->group = 0;
        group1_num--;
        group0_num++;
    }
    cell->status = 1;
    update_gain();
}

void Place::update_gain(){
    int _net_num = cell_list[target].net_pair.size();
    Cell *cell = &cell_list[target];
    int temp = cell->fs;
    cell->fs = cell->te;
    cell->te = temp;
    cell->gain = cell->te - cell->fs;
    for(int i=0; i<_net_num; i++){
        Cell *pair = &cell_list[cell->net_pair[i].cell];
        if(pair->group != cell->group){
            pair->fs += cell->net_pair[i].weight;
            pair->te -= cell->net_pair[i].weight;
        }
        else{
            pair->te += cell->net_pair[i].weight;
            pair->fs -= cell->net_pair[i].weight;
        }
    }
    for(int i=0; i<_net_num; i++){
        Cell *pair = &cell_list[cell->net_pair[i].cell];
        pair->gain = pair->te - pair->fs; 
    }
}

void Place::run_FM(){
    flag_imp = 0;
    while(flag_imp == 0){
        find_max();
        if(flag_imp == 0){
           move_node();
        }
    }
}

void Place::partition(){
    graph_construct();
    set_fs_te();
    calculate_gain();
    run_FM();
    //calculate_cutsize();
}   