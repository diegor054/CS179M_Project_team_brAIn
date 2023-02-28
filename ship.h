#ifndef SHIP_H
#define SHIP_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace std;

class Ship{
    private:
        vector<vector<Container*>> grid;
        double hn;
        double fn;
        double gn;
        Ship* parent;

    public:
        Ship();
        Ship(vector<vector<Container*>>);
        Ship(Ship* p);
        Ship(Ship*, int);
        void print();                                                                      
        vector<pair<int,int>> pickUp();                         
        vector<Ship*> dropDown(pair<int, int>);                 
        vector<pair<Ship*,Container*>> unloadContainer(vector<pair<int, int>> allCont, int index);  
        vector<int> find_num_containers();
        vector<pair<int,int>> find_container_location();
        int find_nearest_col();
        void calculate_hn();
        double getCost(){return fn;}
        void trickleDown();
        void removeContainer(Container*);
        void addContainer(Container*, int);
        vector<vector<Container*>> getGrid();
        void setGrid(vector<vector<Container*>>);
        Ship* move_right(Ship*, pair<int, int> &);
        Ship* move_left(Ship*, pair<int, int> &);
        int containersAbove(Ship*, pair<int, int>);
        Ship* getParent(){return parent;}
        Ship* setParent(Ship* p){parent = p;}
        string outputMoves(Ship* p, Ship* c);
};

struct Container {
    int weight;
    string contents;
    Container() : weight(0), contents("UNUSED") { };
    Container(int w, string c) 
    {
      if (c == "NAN") { weight = 0; }
      else { weight = w; }
      contents = c;
    };
};

#endif