
#include "b_star.h"
#include "module.h"

using namespace std;

random_device rd;
default_random_engine generator( rd() );


/*===============================================================================*/
/*                                   < parser >                                  */
/*===============================================================================*/

void Floorplanner::parser_block(ifstream& in_block){
    int outline_w,outline_h,Num_B,Num_T;
    string temp;
    in_block >> temp >> outline_w >> outline_h;
    in_block >> temp >> Num_B;
    in_block >> temp >> Num_T;
    outline_width = outline_w;
    outline_height = outline_h;
    NumBlocks = Num_B;
    NumTerminals = Num_T;

    int cnt_B=0,cnt_T=0;
    while(cnt_B < Num_B){
        string name;
        double w, h;
        in_block >> name >> w >> h;
        Block* BL = new Block(name, w, h);
        search_block.insert(pair<string, Block*>(name, BL));
        block_set.push_back(BL);
        cnt_B++;
    }
    while(cnt_T < Num_T){
        string name;
        double x, y;
        in_block >> name >> temp >> x >> y ;
        Terminal* TE = new Terminal(name, x, y);
        search_terminal.insert(pair<string, Terminal*>(TE->name, TE));
        terminal_set.push_back(TE);
        cnt_T++;
    }
    //cout<<"blk: "<<block_set.size()<<endl;
    //cout<<"ter: "<<terminal_set.size()<<endl;  
}

void Floorplanner::parser_net(ifstream& in_net){
    int num_net;
    int cnt_n=0;
    string temp_;
    in_net >> temp_ >> num_net;
    NumNets = num_net;

    while(cnt_n < num_net){
        string temp;
        int degree;
        in_net >> temp >> degree;
        Net* NET = new Net(degree);
        NET->include_BandT.resize(degree);
        for(int i=0;i<degree;i++){
            string block_name;
            in_net >> block_name;
            NET->include_BandT[i] = block_name;
        }
        net_set.push_back(NET);
        cnt_n++;
    }
}

/*===============================================================================*/
/*                             < set initial tree >                              */
/*===============================================================================*/

void Floorplanner::set_initial_tree(){
    Node *root_node = new Node(block_set[0]->name, block_set[0]->w,block_set[0]->h);
    root = root_node;
    node_set.push_back(root_node);
    search_node.insert(pair<string, Node*>(block_set[0]->name,root_node));
    Node *temp = root;
    for(int i=1;i<block_set.size();i++){
        Node *temp_node = new Node(block_set[i]->name, block_set[i]->w,block_set[i]->h);
        node_set.push_back(temp_node);
        search_node.insert(pair<string, Node*>(block_set[i]->name,temp_node));
        temp->left = temp_node;
        temp_node->parent = temp;
        temp = temp->left;
    }
}

/*===============================================================================*/
/*                           < calculate HPWL & cost >                           */
/*===============================================================================*/

int Floorplanner::wirelength(Net* net){
    string name_ = net->include_BandT[0];
    double net_min_x, net_min_y, net_max_x, net_max_y;
    if(search_node[name_]){
        Node *temp_node = search_node[name_];
        net_min_x = temp_node->get_center_x();
        net_max_x = temp_node->get_center_x();
        net_min_y = temp_node->get_center_y();
        net_max_y = temp_node->get_center_y();
    }
    else if(search_terminal[name_]){
        Terminal *temp_ter = search_terminal[name_];
        net_min_x = temp_ter->x;
        net_max_x = temp_ter->x;
        net_min_y = temp_ter->y;
        net_max_y = temp_ter->y;
    }
    for(int i=1;i<net->include_BandT.size();i++){
        string temp_name = net->include_BandT[i];
        if(search_node[temp_name]){
            Node *temp_node = search_node[temp_name];
            net_min_x = min(net_min_x, temp_node->get_center_x());
            net_max_x = max(net_max_x, temp_node->get_center_x());
            net_min_y = min(net_min_y, temp_node->get_center_y());
            net_max_y = max(net_max_y, temp_node->get_center_y());
        }
        else if(search_terminal[temp_name]){
            Terminal *temp_ter = search_terminal[temp_name];
            net_min_x = min(net_min_x, temp_ter->x);
            net_max_x = max(net_max_x, temp_ter->x);
            net_min_y = min(net_min_y, temp_ter->y);
            net_max_y = max(net_max_y, temp_ter->y);
        }
    }
    return ((int)net_max_x - (int)net_min_x + (int)net_max_y - (int)net_min_y);
}

int Floorplanner::HPWL(){
    int total_wirelength=0;
    for(int i=0; i<net_set.size(); i++){
        total_wirelength += wirelength(net_set[i]);
    }
    return total_wirelength;
}

double Floorplanner::cal_cost(){
    HPWL_ = HPWL();
    return alpha * max_x * max_y + (1-alpha) * HPWL_;
}

double Floorplanner::cal_area_cost(){
    double x_dis = max(max_x-outline_width, 0.0);
    double y_dis = max(max_y-outline_height, 0.0);
    return (x_dis + y_dis)*10;    
}

/*===============================================================================*/
/*                       < set coordinate & contour line >                       */
/*===============================================================================*/

double Floorplanner::update_contour(Node* node){
    double local_max_y =0;
    double x_ =  node->x;
    double h_ =  node->h;
    double x_w = node->x + node->w;
    list<contour>::iterator it = contour_line.begin();
    while(it!=contour_line.end()){
        if(it->x2 <= x_){
			it++;
		}
        else if(it->x1 >= x_w){
			break;
		}
        else{
            local_max_y = max(local_max_y,it->y);
            if((it->x1 >= x_) && (it->x2 <= x_w)){
                it=contour_line.erase(it);
            }
            else if((it->x1 >= x_) && (it->x2 >= x_w)){
                it->x1 = x_w;
                contour_line.insert(it,contour(x_, x_w, local_max_y + h_));
                return local_max_y;
            }
        }
    }
    contour_line.insert(it,contour(x_, x_w, local_max_y + h_));
    return local_max_y;
}

void Floorplanner::set_coordinate(Node* node){ //pre-order
    if(!root){
        cout<<"warning!"<<endl;
    }
    if(node == NULL){
        return;
    }
    if(node->parent == NULL){//root
        node->x = 0;
        node->y = 0;
        contour p(node->x, node->w, node->h);
        contour_line.push_back(p);  
        max_x = node->w;
        max_y = node->h;
    }
    else if(node == node->parent->left){//horizontal
        node->x = node->parent->x + node->parent->w;
        max_x = max((node->x+node->w), max_x);
        node->y = update_contour(node);
        max_y = max((node->y+node->h), max_y);
    }
    else if(node == node->parent->right){//vertical
        node->x = node->parent->x;
        max_x = max((node->x+node->w), max_x);
        node->y = update_contour(node);
        max_y = max((node->y+node->h), max_y);
    }
    set_coordinate(node->left);
    set_coordinate(node->right);
}

/*===============================================================================*/
/*                         < initialize & check legel >                          */
/*===============================================================================*/

void Floorplanner::check_legal(){
    if((max_x <= outline_width) && (max_y <= outline_height)){
        is_legal = 1;
    }
}

void Floorplanner::initialize(){
    is_legal = 0;
    set_initial_tree();
    set_coordinate(root);
    check_legal();
    if(is_legal == 1){
        min_cost = cal_cost();
    }
    else{
        min_cost = cal_area_cost();
    }
}

void Floorplanner::after_movement(){
    contour_line.clear();
    is_legal = 0;
    set_coordinate(root);
    check_legal();
    if(is_legal == 1){
        min_cost = cal_cost();
    }
    else{
        min_cost = cal_area_cost();
    }
}

/*===============================================================================*/
/*                           < B star tree movement >                            */
/*===============================================================================*/

void Floorplanner::rotate_blk(int blk_num){
    Block* blk = block_set[blk_num];
    Node* node = search_node[blk->name];
    double w_,h_;
    w_ = blk->w;
    h_ = blk->h;
    blk->w = h_ ;
    blk->h = w_ ;
    node->w = h_;
    node->h = w_;
}

void Floorplanner::swap_blk(int blk_num1,int blk_num2){
    Block* blk1 = block_set[blk_num1];
    Block* blk2 = block_set[blk_num2];
    Node* node1 = search_node[blk1->name];
    Node* node2 = search_node[blk2->name];
    string temp_name;
    double temp_w, temp_h;
    temp_name = blk2->name;
    temp_w = blk2->w;
    temp_h = blk2->h;
    blk2->name = blk1->name;
    blk2->w = blk1->w;
    blk2->h = blk1->h;
    blk1->name = temp_name;
    blk1->w = temp_w;
    blk1->h = temp_h;
    node2->name = node1->name;
    node2->w = node1->w;
    node2->h = node1->h;
    node1->name = temp_name;
    node1->w = temp_w;
    node1->h = temp_h;
    search_node[blk1->name] = node1;
    search_node[blk2->name] = node2;
}

void Floorplanner::del_and_ins_blk(int del_,int ins_){
    //blk1 is deleted and insert after blk2
    //delete
    Node* del = node_set[del_];
    if((del->left == NULL) && (del->right == NULL)){
        if(del->parent != NULL){
            if(del->parent->left == del){
                del->parent->left = NULL;
            }
            else if(del->parent->right == del){
                del->parent->right = NULL;
            }
        }
        else{
            root = NULL;
        }
    }
    else if((del->left != NULL) && (del->right == NULL)){
        if(del->parent != NULL){
            del->left->parent = del->parent;
            if(del->parent->left == del){
                del->parent->left = del->left;
            }
            else if(del->parent->right == del){
                del->parent->right = del->left;
            }
        }
        else{
            del->left->parent = NULL;
            root = del->left;
        }
    }
    else if((del->right != NULL) && (del->left == NULL)){
        if(del->parent != NULL){
            del->right->parent = del->parent;
            if(del->parent->left == del){
                del->parent->left = del->right;
            }
            else if(del->parent->right == del){
                del->parent->right = del->right;
            }
        }
        else{
            del->right->parent = NULL;
            root = del->right;
        }
    }
    del->parent = NULL;
    del->left = NULL;
    del->right = NULL;
    //insert
    Node* ins = node_set[ins_];
    if((ins->left == NULL) && (ins->right == NULL)){
        unsigned seed;
        seed = (unsigned)time(NULL);
        srand(seed);
        int r = rand();
        if(r%2 == 0){
            ins->right = del;
            del->parent = ins;
        } 
        else if(r%2 == 1){
            ins->left = del;
            del->parent = ins;
        }
    }
    else if((ins->left == NULL) && (ins->right != NULL)){
        ins->left = del;
        del->parent = ins;
    }
    else if((ins->left != NULL) && (ins->right == NULL)){
        ins->right = del;
        del->parent = ins;
    }
}

void Floorplanner::recover(int del,int ins,int prev_parent,int prev_child,Node* prev_parentBlk,Node* prev_childBlk){
    if(node_set[ins]->left == node_set[del]) node_set[ins]->left=NULL;
    else if(node_set[ins]->right == node_set[del]) node_set[ins]->right=NULL;
    if(prev_parent == 0){
        prev_parentBlk->left = node_set[del];
        node_set[del]->parent = prev_parentBlk;
        if(prev_child == 0){
            node_set[del]->left = prev_childBlk;
            prev_childBlk->parent = node_set[del];
        }
        else if(prev_child == 1){
            node_set[del]->right = prev_childBlk;
            prev_childBlk->parent = node_set[del];
        }
    }
    else if(prev_parent == 1){
        prev_parentBlk->right = node_set[del];
        node_set[del]->parent = prev_parentBlk;
        if(prev_child == 0){
            node_set[del]->left = prev_childBlk;
            prev_childBlk->parent = node_set[del];
        }
        else if(prev_child == 1){
            node_set[del]->right = prev_childBlk;
            prev_childBlk->parent = node_set[del];
        }
    }
    else if(prev_parent == 2){
        node_set[del]->parent = NULL;
        root = node_set[del];
        if(prev_child == 0){
            node_set[del]->left = prev_childBlk;
            prev_childBlk->parent = node_set[del];
        }
        else if(prev_child == 1){
            node_set[del]->right = prev_childBlk;
            prev_childBlk->parent = node_set[del];
        }
    }
}

void Floorplanner::preorder(Node* node){
    if(node == NULL){
        return;
    }
    if(node->parent!=NULL){
        cout<<"parent: "<<node->parent->name<<", ";
    }
    cout<<"preorder: "<<node->name<<endl;
    preorder(node->left);
    preorder(node->right);
}

/*===============================================================================*/
/*                           < Simulated Annealing >                             */
/*===============================================================================*/

void Floorplanner::SA(clock_t start){
    double T = 100000.0;
    double frozen_T = 0.1;
    double dec_rate = 0.95;
    int reject = 0;
    uniform_real_distribution<> exp_dis(0.0,1.0);

    while(1){
        reject = 0;
        for(int i=0; i<1000; i++){
            uniform_int_distribution<> choice(1,3);
            int move = choice(generator);
            int prev_legal,prev_cost;
            prev_legal = is_legal;
            prev_cost = min_cost;

            if(move == 1){ //rotate
                uniform_int_distribution<> rotate_dis(0,NumBlocks-1);
                int rotate_blk_num = rotate_dis(generator);
                rotate_blk(rotate_blk_num);
                after_movement();

                double prob = exp_dis(generator);
                if((prev_legal == 1) && (is_legal == 0)){
                    rotate_blk(rotate_blk_num);
                    after_movement();
                    reject++;
                }
                else if((prev_legal == 0) && (is_legal == 1)){
                    continue;
                }
                else if((prev_cost < min_cost) && (prob >= exp((double)(prev_cost-min_cost)/T) )){
                    rotate_blk(rotate_blk_num);
                    after_movement();
                    reject++;
                }
            }
            else if(move == 2){ //del and insert
                int del;
                int ins;
                while(1){
                    uniform_int_distribution<> del_dis(0,NumBlocks-1);
                    uniform_int_distribution<> ins_dis(0,NumBlocks-1);
                    del = del_dis(generator);
                    ins = ins_dis(generator);
                        if((node_set[del] != node_set[ins])&&(node_set[ins]->left == NULL || node_set[ins] ->right == NULL)&&(node_set[del]->left == NULL || node_set[del]->right == NULL)){
                                        break;     
                        }
                }

                int prev_parent; //0left 1right 2root
                Node* prev_parentBlk ;
                if(node_set[del] == root) prev_parent = 2;
                else{
                    prev_parentBlk= node_set[del]->parent;;
                    if(node_set[del]->parent->left == node_set[del]) prev_parent = 0;
                    else if(node_set[del]->parent->right == node_set[del]) prev_parent = 1;
                }
                int prev_child;//0:left child 1:right child 2:none
                Node* prev_childBlk;
                if((node_set[del]->left != NULL) && (node_set[del]->right == NULL)){
                    prev_child = 0;
                    prev_childBlk = node_set[del]->left;
                } 
                else if((node_set[del]->left == NULL) && (node_set[del]->right != NULL)){
                    prev_child = 1;
                    prev_childBlk = node_set[del]->right;
                } 
                else if((node_set[del]->left == NULL) && (node_set[del]->right == NULL)){
                    prev_child = 2;
                } 

                del_and_ins_blk(del,ins);
                after_movement();

                double prob = exp_dis(generator);
                if((prev_legal == 1) && (is_legal == 0)){
                    recover(del,ins,prev_parent,prev_child,prev_parentBlk,prev_childBlk);
                    after_movement();
                    reject++;
                }
                else if((prev_legal == 0) && (is_legal == 1)){
                    continue;
                }
                else if((prev_cost < min_cost) && (prob >= exp((double)(prev_cost-min_cost)/T) )){
                    recover(del,ins,prev_parent,prev_child,prev_parentBlk,prev_childBlk);
                    after_movement();
                    reject++;
                }
            }
            else if(move == 3){ //swap
                int swap_blk_num1;
                int swap_blk_num2;
                while(1){
                    uniform_int_distribution<> swap_dis1(0,NumBlocks-1);
                    uniform_int_distribution<> swap_dis2(0,NumBlocks-1);
                    swap_blk_num1 = swap_dis1(generator);
                    swap_blk_num2 = swap_dis2(generator);
                    if(swap_blk_num1 != swap_blk_num2){
                        break;
                    }
                }
                swap_blk(swap_blk_num1,swap_blk_num2);
                after_movement();

                double prob = exp_dis(generator);
                if((prev_legal == 1) && (is_legal == 0)){
                    swap_blk(swap_blk_num2,swap_blk_num1);
                    after_movement();
                    reject++;
                }
                else if((prev_legal == 0) && (is_legal == 1)){
                    continue;
                }
                else if((prev_cost < min_cost) && (prob >= exp((double)(prev_cost-min_cost)/T) )){
                    swap_blk(swap_blk_num2,swap_blk_num1);
                    after_movement();
                    reject++;
                }
            }
        }
        T *= dec_rate;
        double rejectRate = (double)reject/1000;
        clock_t moment;
        moment = clock();
        if((double(moment-start)/ CLOCKS_PER_SEC) > 280){
            return;
        }
        if(T <= frozen_T && is_legal == 0){
            T = 9999;
        }
        else if((rejectRate > 0.985) && (is_legal == 1)){
            break;
        }
        else if((T < frozen_T) && (is_legal == 1) ){
            break;
        } 
    }
}

/*===============================================================================*/
/*                                  < output >                                   */
/*===============================================================================*/

void Floorplanner::print_rpt(ofstream& out,clock_t start){
    out<<int(min_cost)<<endl;
    out<<HPWL_<<endl;
    out<<(int)max_x * (int)max_y<<endl;
    out<<max_x<<" "<<max_y<<endl;
    clock_t run_time;
    run_time = clock();
    //cout<<"time: "<<double(run_time-start)/ CLOCKS_PER_SEC<<"s"<<endl;
    out<<double(run_time-start)/ CLOCKS_PER_SEC<<endl;
    for(int i=0;i<block_set.size();i++){
        string str =  block_set[i]->name;
        out<<str<<" "<<search_node[str]->x<<" "<<search_node[str]->y<<" "<<search_node[str]->x+search_node[str]->w<<" "<<search_node[str]->y+search_node[str]->h<<endl;
    }
}