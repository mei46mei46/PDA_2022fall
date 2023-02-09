#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm> 
#include <cstdlib>
#include <stdio.h>
#include <time.h> 

using namespace std;

class Tile{
public:
    int lx,ly,hx,hy;
    int type; //block=0,space=1
    int num;
    Tile(int a=0,int b=0,int c=0,int d=0,int t=0,int n=0):lx(a),ly(b),hx(c),hy(d),type(t),num(n){};
    ~Tile(){};
};
struct block_neighbor{
    int block;
    int space;
    int index;
};
struct point{
    int x;
    int y;
};
bool compare(Tile* a,Tile* b){
    return (a->hy)>(b->hy);
}
bool compare1(Tile* a,Tile* b){
    if((a->lx)==(b->lx)){
        return (a->ly)<(b->ly);
    }
    else{
        return (a->lx)<(b->lx);
    }
}
bool compare2(block_neighbor a,block_neighbor b){
    return (a.index)<(b.index);
}

void insert_block(vector<Tile*> &space,vector<Tile*> &block,int num){
    vector<Tile*> clip;
    vector<Tile*> clip_LR;
    for(int i=0;i<space.size();i++){
        if((space[i]->ly>=block[num]->ly) && (space[i]->ly<block[num]->hy) && (space[i]->lx<=block[num]->lx) && (space[i]->hx>=block[num]->hx)){
            clip.push_back(space[i]);
        }
        else if((space[i]->hy>block[num]->ly) && (space[i]->hy<=block[num]->hy) && (space[i]->lx<=block[num]->lx) && (space[i]->hx>=block[num]->hx)){
            clip.push_back(space[i]);
        }
        else if((space[i]->ly<=block[num]->ly) && (space[i]->hy>=block[num]->hy) && (space[i]->lx<=block[num]->lx) && (space[i]->hx>=block[num]->hx)){
            clip.push_back(space[i]);
        }
    }
    //spilt top & bottom
    sort(clip.begin(), clip.end(), compare);
    if(clip.size()>1){
        for(int i=1;i<clip.size()-1;i++){
            clip_LR.push_back(clip[i]);
        }
    }
    if(clip.size()==1){
        if((clip[0]->hy == block[num]->hy)&&(clip[0]->ly == block[num]->ly)){
            clip_LR.push_back(clip[0]);
        }
        else if(clip[0]->hy == block[num]->hy){
            Tile *new_mid;
            new_mid = new Tile(clip[0]->lx, block[num]->ly, clip[0]->hx, clip[0]->hy, 1, 0);
            space.push_back(new_mid);
            clip[0]->hy = block[num]->ly;
            clip_LR.push_back(new_mid);
        }
        else if(clip[0]->ly == block[num]->ly){
            Tile *new_mid;
            new_mid = new Tile(clip[0]->lx, clip[0]->ly, clip[0]->hx, block[num]->hy, 1, 0);
            space.push_back(new_mid);
            clip[0]->ly = block[num]->hy;
            clip_LR.push_back(new_mid);
        }
        else{
            Tile *new_top,*new_mid;
            new_top = new Tile(clip[0]->lx, block[num]->hy, clip[0]->hx, clip[0]->hy, 1, 0);
            new_mid = new Tile(clip[0]->lx, block[num]->ly, clip[0]->hx, block[num]->hy, 1, 0);
            clip[0]->hy = block[num]->ly;
            space.push_back(new_top);
            space.push_back(new_mid);
            clip_LR.push_back(new_mid);
        }
    }
    else{
        Tile *clip_bottom,*clip_top;
        clip_bottom = clip[clip.size()-1];
        clip_top = clip[0];
        if((clip_top->hy == block[num]->hy)&&(clip_bottom->ly == block[num]->ly)){
            clip_LR.push_back(clip_top);
            clip_LR.push_back(clip_bottom);
        }
        else if(clip[0]->hy == block[num]->hy){
            Tile *new_bottom;
            new_bottom = new Tile(clip_bottom->lx, block[num]->ly, clip_bottom->hx, clip_bottom->hy, 1, 0);
            clip_bottom->hy = block[num]->ly;
            clip_LR.push_back(new_bottom);
            clip_LR.push_back(clip_top);
            space.push_back(new_bottom);
        }
        else if(clip_bottom->ly == block[num]->ly){
            Tile *new_top;
            new_top = new Tile(clip[0]->lx, clip[0]->ly, clip[0]->hx, block[num]->hy, 1, 0);
            clip_top->ly = block[num]->hy;
            clip_LR.push_back(new_top);
            clip_LR.push_back(clip_bottom);
            space.push_back(new_top);
        }
        else{
            Tile *new_top;
            new_top = new Tile(clip[0]->lx, clip[0]->ly, clip[0]->hx, block[num]->hy, 1, 0);
            clip_top->ly = block[num]->hy;
            clip_LR.push_back(new_top);
            space.push_back(new_top);

            Tile *new_bottom;
            new_bottom = new Tile(clip_bottom->lx, block[num]->ly, clip_bottom->hx, clip_bottom->hy, 1, 0);
            clip_bottom->hy = block[num]->ly;
            clip_LR.push_back(new_bottom);
            space.push_back(new_bottom);
        }
    }
    //spilt left & right
    vector<Tile*> wait_merge;
    for(int i=0;i<clip_LR.size();i++){
        if((clip_LR[i]->hx == block[num]->hx) && (clip_LR[i]->lx == block[num]->lx)){
            vector<Tile*>::iterator it = std::find(space.begin(), space.end(), clip_LR[i]);
            if(it != space.end()){
                space.erase(it);
            }
        }
        else if(clip_LR[i]->hx == block[num]->hx){
            clip_LR[i]->hx = block[num]->lx;
            wait_merge.push_back(clip_LR[i]);
        }
        else if(clip_LR[i]->lx == block[num]->lx){
            clip_LR[i]->lx = block[num]->hx;
            wait_merge.push_back(clip_LR[i]);
        }
        else{
            Tile *new_right;
            new_right = new Tile(block[num]->hx, clip_LR[i]->ly, clip_LR[i]->hx, clip_LR[i]->hy, 1, 0);
            clip_LR[i]->hx = block[num]->lx;
            space.push_back(new_right);
            wait_merge.push_back(clip_LR[i]);
            wait_merge.push_back(new_right);
        }
    }
    //merge
    sort(space.begin(), space.end(), compare);
    sort(wait_merge.begin(), wait_merge.end(), compare);
    for(int i=0;i<wait_merge.size();i++){  
        vector<Tile*>::iterator it = std::find(space.begin(), space.end(), wait_merge[i]);
        if(it == space.end()){
            continue;
        }
        for(int j=0;j<space.size();j++){
            if(space[j]->hy < wait_merge[i]->ly){
                break;
            }
            else if((space[j]->lx == wait_merge[i]->lx) &&(space[j]->hx == wait_merge[i]->hx) && (space[j]!=wait_merge[i])){
                if(space[j]->ly == wait_merge[i]->hy){
                    wait_merge[i]->hy = space[j]->hy;
                    space.erase(space.begin()+j);
                    j--;
                }
                else if(space[j]->hy == wait_merge[i]->ly){
                    wait_merge[i]->ly = space[j]->ly;
                    space.erase(space.begin()+j);
                    j--;
                }
            }
        }
    }
}

void point_finding(vector<Tile*> space,vector<Tile*> block,int x,int y,vector<point> &finding,int outline_width,int outline_height){
    point temp;
    for(int i=0;i<space.size();i++){
        if((x >= space[i]->lx) && (x < space[i]->hx)){
            if((y >= space[i]->ly) && (y < space[i]->hy)){
                temp.x = space[i]->lx;
                temp.y = space[i]->ly;
                finding.push_back(temp);
                return;
            }
        }
        else if((x == space[i]->hx) && (x == outline_width)){
                temp.x = space[i]->lx;
                temp.y = space[i]->ly;
                finding.push_back(temp);
                return;
        }
        else if((y == space[i]->hy) && (y == outline_height)){
                temp.x = space[i]->lx;
                temp.y = space[i]->ly;
                finding.push_back(temp);
                return;
        }
    }
    for(int i=0;i<block.size();i++){
        if((x >= block[i]->lx) && (x < block[i]->hx)){
            if((y >= block[i]->ly) && (y < block[i]->hy)){
                temp.x = block[i]->lx;
                temp.y = block[i]->ly;
                finding.push_back(temp);
                return;
            }
        }
        else if((x == block[i]->hx) && (x == outline_width)){
            temp.x = block[i]->lx;
            temp.y = block[i]->ly;
            finding.push_back(temp);
            return;
        } 
        else if((y == block[i]->hy) && (y == outline_height)){
            temp.x = block[i]->lx;
            temp.y = block[i]->ly;
            finding.push_back(temp);
            return;
        }
    }
}

void neighbor_finding(vector<Tile*> space,vector<Tile*> block,vector<block_neighbor> &block_count){
    sort(space.begin(), space.end(), compare1);
    sort(block.begin(), block.end(), compare1);
    for(int i=0;i<block.size();i++){
        int b_count=0,s_count=0;
        for(int j=0;j<block.size();j++){            
            if(block[j]->lx > block[i]->hx){
                break;
            }
            else{
                //tr
                if(block[j]->lx == block[i]->hx){
                    if((block[j]->hy > block[i]->ly) && (block[j]->hy <= block[i]->hy)){
                        b_count++;
                    }
                    else if((block[j]->ly >= block[i]->ly) && (block[j]->ly < block[i]->hy)){
                        b_count++;
                    }
                    else if((block[j]->ly < block[i]->ly) && (block[j]->hy > block[i]->hy)){
                        b_count++;
                    }
                }
                //bl
                if(block[j]->hx == block[i]->lx){
                    if((block[j]->hy > block[i]->ly) && (block[j]->hy <= block[i]->hy)){
                        b_count++;
                    }
                    else if((block[j]->ly >= block[i]->ly) && (block[j]->ly < block[i]->hy)){
                        b_count++;
                    }
                    else if((block[j]->ly < block[i]->ly) && (block[j]->hy > block[i]->hy)){
                        b_count++;
                    }
                }
                //rt
                if(block[j]->ly == block[i]->hy){
                    if((block[j]->hx > block[i]->lx) && (block[j]->hx <= block[i]->hx)){
                        b_count++;
                    }
                    else if((block[j]->lx >= block[i]->lx) && (block[j]->lx < block[i]->hx)){
                        b_count++;
                    }
                    else if((block[j]->lx < block[i]->lx) && (block[j]->hx > block[i]->hx)){
                        b_count++;
                    }
                }
                //lb
                if(block[j]->hy == block[i]->ly){
                    if((block[j]->hx > block[i]->lx) && (block[j]->hx <= block[i]->hx)){
                        b_count++;
                    }
                    else if((block[j]->lx >= block[i]->lx) && (block[j]->lx < block[i]->hx)){
                        b_count++;
                    }
                    else if((block[j]->lx < block[i]->lx) && (block[j]->hx > block[i]->hx)){
                        b_count++;
                    }
                }
            }
        }
        for(int j=0;j<space.size();j++){
            if(space[j]->lx > block[i]->hx){
                break;
            }
            else{
                //tr
                if(space[j]->lx == block[i]->hx){
                    if((space[j]->hy > block[i]->ly) && (space[j]->hy <= block[i]->hy)){
                        s_count++;
                    }
                    else if((space[j]->ly >= block[i]->ly) && (space[j]->ly < block[i]->hy)){
                        s_count++;
                    }
                    else if((space[j]->ly < block[i]->ly) && (space[j]->hy > block[i]->hy)){
                        s_count++;
                    }
                }
                //bl
                if(space[j]->hx == block[i]->lx){
                    if((space[j]->hy > block[i]->ly) && (space[j]->hy <= block[i]->hy)){
                        s_count++;
                    }
                    else if((space[j]->ly >= block[i]->ly) && (space[j]->ly < block[i]->hy)){
                        s_count++;
                    }
                    else if((space[j]->ly < block[i]->ly) && (space[j]->hy > block[i]->hy)){
                        s_count++;
                    }
                }
                //rt
                if(space[j]->ly == block[i]->hy){
                    if((space[j]->hx > block[i]->lx) && (space[j]->hx <= block[i]->hx)){
                        s_count++;
                    }
                    else if((space[j]->lx >= block[i]->lx) && (space[j]->lx < block[i]->hx)){
                        s_count++;
                    }
                    else if((space[j]->lx < block[i]->lx) && (space[j]->hx > block[i]->hx)){
                        s_count++;
                    }
                }
                //lb
                if(space[j]->hy == block[i]->ly){
                    if((space[j]->hx > block[i]->lx) && (space[j]->hx <= block[i]->hx)){
                        s_count++;
                    }
                    else if((space[j]->lx >= block[i]->lx) && (space[j]->lx < block[i]->hx)){
                        s_count++;
                    }
                    else if((space[j]->lx < block[i]->lx) && (space[j]->hx > block[i]->hx)){
                        s_count++;
                    }
                }
            }
        }
        block_neighbor temp;
        temp.index = block[i]->num;
        temp.block = b_count;
        temp.space = s_count;
        block_count.push_back(temp);
    }
}

int main(int argc,char *argv[]){
    /*clock_t end,start;
    start = clock();*/

    ifstream in;
    in.open(argv[1]);
    ofstream out;
    out.open(argv[2]);

    vector<Tile*> space;
    vector<Tile*> block;
    vector<point> finding;
    vector<block_neighbor> block_count;

    int outline_width,outline_height;
    in >> outline_width >> outline_height; 
    Tile *temp_tile;
    temp_tile = new Tile(0,0,outline_width,outline_height,1,0);   
    space.push_back(temp_tile);

    char buffer[256]={0};
    int temp;
    int num=0;
    int lx,ly,w,h,block_num;
    int x,y;
    in.getline(buffer,256);    
    while(in.getline(buffer,256)){
        if(buffer[0] != 'P'){
            sscanf(buffer,"%d %d %d %d %d\n",&block_num,&lx,&ly,&w,&h);
            Tile* temp_block;
            temp_block = new Tile(lx,ly,lx+w,ly+h,0,block_num);
            block.push_back(temp_block);
            insert_block(space,block,num);
            num++;         
        }
        else{           
            sscanf(buffer,"P %d %d\n",&x,&y);
            point_finding(space,block,x,y,finding,outline_width,outline_height);
        }
    }
    neighbor_finding(space,block,block_count);
    sort(block_count.begin(),block_count.end(),compare2);
    out<<space.size()+block.size()<<endl;
    for(int i=0;i<block.size();i++){
        out<<block_count[i].index<<" "<<block_count[i].block<<" "<<block_count[i].space<<endl;
    }
    for(int i=0;i<finding.size();i++){
        out<<finding[i].x<<" "<<finding[i].y<<endl;
    }
    in.close();
    /*end = clock();
    cout<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;*/
    return 0;
}