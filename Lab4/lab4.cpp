#include <bits/stdc++.h>

using namespace std;

/*===================== struct =====================*/
struct input{
    input(int a,int b){ top = a; bot = b;};
    int top, bot;
};
struct line{
    line(char d,int _x,int _y,int _last){ direct = d; x = _x; y = _y; last = _last;};
    char direct;
    int x, y, last;
};
struct row{
    row(){ ver = -1; hor = -1;};
    int ver, hor;
};
struct net{
    net(int a){ idx = a;};
    int idx;
    vector<line*> lines;
};
struct node{
    node(int a){next = NULL; idx = a;}
    node* next;
    int idx;
};
struct hseg{
    vector<int> vec;
};

/*===================== variable =====================*/
vector<input> cols;
vector<net> nets;
int MAX, len;
int up_bound, low_bound;
vector<int> num1,num2;
vector<int> pin_fin;
vector< vector<row> > maze;
int row_num;
set<int> step1_check,step2up_check,step2bot_check,step4_check;
map<int, int> idx2NetVecIdx;
map<int, int> idx2Up_choose, idx2Bot_choose;
map<int, int> Up_choose2idx, Bot_choose2idx;
vector<int> up_order, down_order, up_idx;
vector<line*> need_change1, need_change2, need_change3,need_change4, need_change5;
vector<hseg> h_seg;
vector<node*> tree;

/*===================== function =====================*/
void parser(ifstream& in);
void step1();
void step2_up();
void print_choosePin();
void step2_down();
void step3();
void step3_changePoint(int a);
void step4_changePoint(int a);
void step4();
void step3_h_seg();
void output(ofstream& out);

/*===================== main =====================*/
int main(int argc, char *argv[]){
    ifstream in;
    ofstream out;
    in.open(argv[1]);
    out.open(argv[2]);
    parser(in);
    in.close();
    row_num = 0;

    step1();
    step2_up();
    step2_down();
    step3();
    step4();
    //print_choosePin();
    output(out);
    return 0;
}

/*===================== function =====================*/

void print_choosePin(){
    for(int i=0;i<up_order.size(); i++){
        cout<<up_order[i]<<", "<<idx2Up_choose[up_order[i]]<<endl;
    }
    cout<<endl;
    for(int i=0;i<down_order.size(); i++){
        cout<<down_order[i]<<", "<<idx2Bot_choose[down_order[i]]<<endl;
    }
}

void output(ofstream& out){
    for(int i=0;i<nets.size(); i++){
        if(nets[i].lines.size()!=0){
            out<<".begin "<<nets[i].idx<<endl;
            for(int j=0;j<nets[i].lines.size();j++){
                if(nets[i].lines[j]->direct == 'V' && nets[i].lines[j]->y!=nets[i].lines[j]->last){
                    if((row_num-nets[i].lines[j]->y+1) < ((row_num-nets[i].lines[j]->last)+1)){
                        out<<"."<<nets[i].lines[j]->direct<<" "<<nets[i].lines[j]->x<<" "<<(row_num-nets[i].lines[j]->y)<<" "<<((row_num-nets[i].lines[j]->last))<<endl;
                    }
                    else{
                        out<<"."<<nets[i].lines[j]->direct<<" "<<nets[i].lines[j]->x<<" "<<(row_num-nets[i].lines[j]->last)<<" "<<((row_num-nets[i].lines[j]->y))<<endl;
                    } 
                }
                else if(nets[i].lines[j]->direct == 'H' && nets[i].lines[j]->x!=nets[i].lines[j]->last){
                    if(nets[i].lines[j]->x < nets[i].lines[j]->last){
                        out<<"."<<nets[i].lines[j]->direct<<" "<<nets[i].lines[j]->x<<" "<<(row_num-nets[i].lines[j]->y)<<" "<<nets[i].lines[j]->last<<endl;
                    }
                    else{
                        out<<"."<<nets[i].lines[j]->direct<<" "<<nets[i].lines[j]->last<<" "<<(row_num-nets[i].lines[j]->y)<<" "<<nets[i].lines[j]->x<<endl;
                    }  
                }  
            }
            out<<".end"<<endl;
        }
    }
}

void parser(ifstream& in){
    string line1,line2;
    getline(in,line1);
    getline(in,line2);
    istringstream is1(line1);
    istringstream is2(line2);
    int _up, _down;
    MAX = 0;
    while(is1 >> _up) {
        is2 >> _down;
        input col(_up,_down);
        cols.push_back(col);
        MAX = max(_up, MAX);
        MAX = max(_down, MAX);
    }
    len = cols.size();
    num1.resize(MAX+1);
    num2.resize(MAX+1);
    for(int i=0; i<=MAX; i++){
        num1[i]=0;
        num2[i]=0;
    }
    for(int i=0; i<len; i++){
        num1[cols[i].top]++;
        num2[cols[i].bot]++;
    }
    for(int i=1;i<=MAX;i++){
        if(num1[i]!=0 || num2[i]!=0){
            net _net(i);
            nets.push_back(_net);
            idx2NetVecIdx.insert(pair<int, int>(i,nets.size()-1));
        }
    }
    for(int i=0;i<=MAX;i++){
        node* n = new node(i);
        tree.push_back(n);
    }
}

void step1(){
    int now_row = 0;
    int sx, fx;
    step1_check.insert(0);
    for(int i=0; i<len; i++){
        if(!step1_check.count(cols[i].top)){
            if(num1[cols[i].top] == 1){
                step1_check.insert(cols[i].top);
                continue;
            }
            vector<int> v_pin;
            v_pin.push_back(i);
            for(int j=i+1; j<len; j++){
                if(cols[i].top == cols[j].top){
                    fx = j;
                    v_pin.push_back(j);
                }
            }
            row_num++;
            sx = i;
            int net_idx = idx2NetVecIdx[cols[i].top];
            for(int j=0;j<v_pin.size();j++){
                line* line1 = new line('V', v_pin[j], 0, row_num);
                nets[net_idx].lines.push_back(line1);
            }
            line* line3 = new line('H', sx, row_num, fx);
            nets[net_idx].lines.push_back(line3);
            step1_check.insert(cols[i].top);
        }
    }
}

void step2_up(){
    row_num++;
    up_bound = row_num;
    step2up_check.insert(0);
    int sy, fy;
    for(int i=0; i<len; i++){
        if(!step2up_check.count(cols[i].top) && num2[cols[i].top] != 0){
            up_order.push_back(cols[i].top);
            up_idx.push_back(i);
            idx2Up_choose.insert(pair<int, int>(cols[i].top, i));
            Up_choose2idx.insert(pair<int, int>(i, cols[i].top));
            if(num1[cols[i].top] == 1){
                int net_idx = idx2NetVecIdx[cols[i].top];
                line* line1 = new line('V', i, 0, up_bound);
                nets[net_idx].lines.push_back(line1);
            }
            else{
                int net_idx = idx2NetVecIdx[cols[i].top];
                int tmp_y = nets[net_idx].lines[0]->last;
                line* line1 = new line('V', i, tmp_y, up_bound);
                nets[net_idx].lines.push_back(line1);
            }
            step2up_check.insert(cols[i].top);
        }
    }
}

void step2_down(){
    step2bot_check.insert(0);
    for(int i=0; i<len; i++){
        if(!step2bot_check.count(cols[i].bot) && num1[cols[i].bot] != 0){
            down_order.push_back(cols[i].bot);
            auto iter = Up_choose2idx.find(i);
            if(iter == Up_choose2idx.end()){
                idx2Bot_choose.insert(pair<int, int>(cols[i].bot, i));
                Bot_choose2idx.insert(pair<int, int>(i, cols[i].bot));
            }
            else{
                int fin_num = 0;
                bool fin = 0;
                for(int j=i+1; j<len; j++){
                    if(cols[i].bot == cols[j].bot){
                        fin_num = j;
                        iter = Up_choose2idx.find(j);
                        if(iter == Up_choose2idx.end()){
                            idx2Bot_choose.insert(pair<int, int>(cols[i].bot, j));
                            Bot_choose2idx.insert(pair<int, int>(j, cols[i].bot));
                            fin = 1;
                            break;
                        } 
                    }
                }
                if(fin_num == 0){
                    fin_num = i;
                }
                if(!fin){
                    idx2Bot_choose.insert(pair<int, int>(cols[i].bot, fin_num));
                    Bot_choose2idx.insert(pair<int, int>(fin_num, cols[i].bot));
                }
            }
            step2bot_check.insert(cols[i].bot);
        }
    }
    for(int i=0;i<up_order.size();i++){
        int pin = idx2Up_choose[up_order[i]];
        auto iter = Bot_choose2idx.find(pin); 
        if(iter != Bot_choose2idx.end()){
            int h = Bot_choose2idx[pin];
            tree[up_order[i]]->next = tree[h];
        }
    }
}

void step3(){
    bool h_done = 0;
    queue<int> connect, last;
    int now_row = 0;
    low_bound = row_num;
    up_bound = row_num;
    vector<int> find1;
    for(int i=0; i<up_order.size(); i++){
        auto it = find(find1.begin(), find1.end(), up_order[i]);
        if(it == find1.end()){
            int pin = idx2Up_choose[up_order[i]];
            int pin_b = idx2Bot_choose[up_order[i]];
            auto iter = Bot_choose2idx.find(pin);
            if(iter == Bot_choose2idx.end() && cols[up_idx[i]].bot!=0){//can route
                connect.push(up_order[i]);
            }
            else if(cols[up_idx[i]].bot == 0){//0 at bottom
                last.push(up_order[i]);
            }
            else if(pin == pin_b){//bot=top, can route
                connect.push(up_order[i]);
            }
            else{
                int a = up_order[i];
                hseg hs;
                hs.vec.push_back(a);
                while(tree[a]->next != NULL){
                    if(tree[a]->next->idx == up_order[i]){
                        for(int j=0;j<hs.vec.size();j++){
                            find1.push_back(hs.vec[j]);
                        }
                        h_seg.push_back(hs);
                        break;
                    }
                    else{
                        a = tree[a]->next->idx;
                        hs.vec.push_back(a);
                    }
                }
            }
        }
    }
    /*cout<<"hseg"<<endl;
    for(int i=0;i<h_seg.size();i++){
        for(int j=0;j<h_seg[i].vec.size();j++){
            cout<<h_seg[i].vec[j]<<", ";
        }
        cout << endl;
    }
    cout<<endl;*/
    if(connect.empty() && h_done==0){
        step3_h_seg();
        h_done = 1;
        int size = last.size();
        for (int i = 0; i < size; i++) {
            connect.push(last.front());
            last.pop();
        }
    }
    while(!connect.empty()){//!connect.empty()
        int x = idx2Up_choose[connect.front()];
        int pin = idx2Bot_choose[connect.front()];
        int net_idx = idx2NetVecIdx[connect.front()];
        if(pin == x){
            int tmp_y = nets[net_idx].lines[0]->last;
            line* line4 = new line('V', x, tmp_y, row_num);
            nets[net_idx].lines.push_back(line4);
            need_change4.push_back(nets[net_idx].lines.back());
        }
        else{
            row_num++;
            int choose_track = up_bound+1;
            step3_changePoint(choose_track);
            low_bound = row_num;
            line* line1 = new line('V', x, up_bound, choose_track);
            nets[net_idx].lines.push_back(line1);
            need_change1.push_back(nets[net_idx].lines.back());
            if(choose_track != low_bound){
                line* line2 = new line('V', pin, choose_track, low_bound);
                nets[net_idx].lines.push_back(line2);
                need_change2.push_back(nets[net_idx].lines.back());
            }
            line* line3 = new line('H', x, choose_track, pin);
            nets[net_idx].lines.push_back(line3);
            need_change1.push_back(nets[net_idx].lines.back());
        }
        pin_fin.push_back(connect.front());
        auto iter = Up_choose2idx.find(pin);
        if(iter != Up_choose2idx.end()){
            int top = Up_choose2idx[pin];
            auto iter1 = find(pin_fin.begin(),pin_fin.end(),top);
            if(iter1 == pin_fin.end()){
                connect.push(top);
            }
        }
        connect.pop();
        if(connect.empty() && h_done==0){
            step3_h_seg();
            h_done = 1;
            int size = last.size();
            for (int i = 0; i < size; i++) {
                connect.push(last.front());
                last.pop();
            }
        }
    }
}

void step3_changePoint(int a){
    for(int i=0; i<need_change1.size(); i++){
        if(need_change1[i]->direct == 'H'){
            need_change1[i]->y++;
        }
        else{
            need_change1[i]->last++;
        }
    }
    for(int i=0; i<need_change2.size(); i++){
        need_change2[i]->y++;
        need_change2[i]->last++;
    }
    for(int i=0; i<need_change4.size(); i++){
        need_change4[i]->last++;
    }
}

void step3_h_seg(){
    for(int j=0; j<h_seg.size();j++){
        int n1 = h_seg[j].vec[0],n2 = h_seg[j].vec[1];
        int x1 = idx2Up_choose[n1], x2 = idx2Up_choose[n2];
        int use_zero;
        for(int i=0;i<len;i++){
            if(cols[i].bot==0 || cols[i].top==0){
                use_zero = i;
                break;
            }
        }
        row_num++;
        row_num++;
        row_num++;
        int now_row = up_bound+1;
        low_bound = row_num-1;
        int net_idx = idx2NetVecIdx[n1];
        int net_idx2 = idx2NetVecIdx[n2];
        step3_changePoint(now_row+2);
        step3_changePoint(now_row+1);
        step3_changePoint(now_row);
        //1
        line* line1 = new line('V', x1, up_bound, now_row);
        nets[net_idx].lines.push_back(line1);
        need_change1.push_back(line1);
        //2
        line* line2 = new line('H', x1, now_row, use_zero);
        nets[net_idx].lines.push_back(line2);
        need_change1.push_back(line2);
        //3
        line* line3 = new line('V', use_zero, now_row, now_row+2);
        nets[net_idx].lines.push_back(line3);
        need_change2.push_back(line3);
        //4
        line* line4 = new line('H', x2, now_row+2, use_zero);
        nets[net_idx].lines.push_back(line4);
        need_change1.push_back(line4);
        //5
        line* line5 = new line('V', x2, now_row+2, row_num);
        nets[net_idx].lines.push_back(line5);
        need_change2.push_back(line5);
        //6
        line* line6 = new line('V', x2, up_bound, now_row+1);
        nets[net_idx2].lines.push_back(line6);
        need_change1.push_back(line6);
        //7
        line* line7 = new line('H', x2, now_row+1, x1);
        nets[net_idx2].lines.push_back(line7);
        need_change1.push_back(line7);
        //8
        line* line8 = new line('V', x1, now_row+1, row_num);
        nets[net_idx2].lines.push_back(line8);
        need_change2.push_back(line8);
    }
}

void step4(){
    int sx, fx=-1;
    step4_check.insert(0);
    row_num++;
    low_bound = row_num-1;
    for(int i=0; i<len; i++){
        if(!step4_check.count(cols[i].bot)){
            sx = i;
            int net_idx = idx2NetVecIdx[cols[i].bot];
            int bot_choose = idx2Bot_choose[cols[i].bot];
            vector<int> v_pin;
            v_pin.push_back(i);
            for(int j=i+1; j<len; j++){
                if(cols[i].bot == cols[j].bot){
                    fx = j;
                    v_pin.push_back(j);
                }
            }
            if(v_pin.size() == 1){
                line* line1 = new line('V', v_pin[0], low_bound, row_num);
                nets[net_idx].lines.push_back(line1);
                need_change3.push_back(line1);
                step4_check.insert(cols[i].bot);
                continue;
            }
            row_num++;
            int choose_track = row_num;
            if(!need_change3.empty()){
                step4_changePoint(choose_track);
            }
            for(int j=0;j<v_pin.size();j++){
                if(v_pin[j] == bot_choose && num1[cols[i].bot]!=0){
                    line* line1 = new line('V', v_pin[j], low_bound, row_num);
                    nets[net_idx].lines.push_back(line1);
                    need_change3.push_back(line1);
                }
                else{
                    line* line1 = new line('V', v_pin[j], choose_track, row_num);
                    nets[net_idx].lines.push_back(line1);
                    need_change3.push_back(line1);
                }
                
            }
            if(num2[cols[i].bot]!=1){
                line* line3 = new line('H', sx, choose_track, fx);
                nets[net_idx].lines.push_back(line3);
            }
            step4_check.insert(cols[i].bot);
        }
    }
    row_num++;
    if(!need_change3.empty()){
        step4_changePoint(1);
    }
}

void step4_changePoint(int a){
    for(int i=0; i<need_change3.size(); i++){
        need_change3[i]->last++;
    }
}