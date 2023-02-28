#include "ship.h"
using namespace std;

Ship::Ship(){
    grid =
    {
        {new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED")},      // 00 01 02 03 04 05
        {new Container(0, "UNUSED"),new Container(5, "UNUSED"),new Container(0, "UNUSED"),new Container(8, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(9, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED")}      // 10 11 12 13 14 15
    };
    setUniqueKey();
    fn = 1;
    parent = NULL;
}

Ship::Ship(vector<vector<Container*>> p){
    grid = p;
    parent = NULL;
    calculate_hn();
}

Ship::Ship(Ship* p){
    grid = p->grid;
    parent = p;
    gn = p->gn + 1;
    calculate_hn();
    setUniqueKey();
}

Ship::Ship(Ship* p, int fyou){
    grid = p->grid;
    gn = p->gn;
    calculate_hn();
    parent = NULL;
}

void Ship::print(){
    for(int i =0; i < grid.size(); ++i ){
        for(int j = 0; j < grid.at(0).size(); j++){
            cout << grid.at(i).at(j)->weight << '\t';
        }
        cout << endl;
    }
    cout << endl;
}

vector<pair<int,int>> Ship::pickUp(){
    vector<int> filled(grid[0].size(), -1);
    vector<pair<int, int>> pickupIdxs(grid[0].size(), make_pair(-1, -1));

    for(int i = 0; i < grid.size(); ++i){
        vector<Container*> row = grid[i];
        for(int j = 0; j < row.size(); ++j){
            if(grid[i][j]->weight > 0 && filled[j] == -1){
                pickupIdxs[j] = make_pair(i, j);
                filled[j] = 1;
            }
        }
    }
    for(int i = 0; i < pickupIdxs.size(); ++i){
        pickupIdxs.erase(remove(pickupIdxs.begin(), pickupIdxs.end(), make_pair(-1,-1)), pickupIdxs.end());
    }
    return pickupIdxs;
}

vector<Ship*> Ship::dropDown(pair<int, int> idx){
    vector<Ship*> children;
    Ship *move = new Ship(grid);
    pair<int, int> left = idx;
    pair<int, int> right = idx;
    int size = 12;
    int i = 0;

    while(size > 0){
        move = move_right(move, right);
        // cout << "done" << endl;
        if(move == NULL){
            size--;
            continue;
        }
        children.push_back(new Ship(move, 0));
        size--;
    }
    size = 12;
    move = new Ship(grid);
    while(size > 0){
        move = move_left(move, left);
        if(move == NULL){
            size--;
            continue;
        }
        children.push_back(new Ship(move, 0));
        size--;
    }

    for(int i = 0; i < children.size(); ++i){
        children.at(i)->parent = this;
    }
    return children;
}

vector<pair<Ship*,Container*>> Ship::unloadContainer(vector<pair<int, int>> containers, int index){
    vector<pair<Ship*,Container*>> children;
    pair<int,int> idx = containers[index];
    Ship *move = new Ship(grid);
    int above = containersAbove(this, idx);
    bool isAboveContainerToUnload = true;
    Container* nullCont = NULL;
    Ship* nullShip = NULL;

    while(above > 0){
        pair<int, int> idxOfTopContainer = make_pair(idx.first-above,idx.second);
        Container* cont = move->grid[idxOfTopContainer.first][idxOfTopContainer.second];

        move = move_left(move, idxOfTopContainer);
        isAboveContainerToUnload = true;

        if(move == NULL){
            children.push_back(make_pair(nullShip, new Container(cont->weight, cont->contents)));
            above--;
            continue;
        }
        while(isAboveContainerToUnload){
            isAboveContainerToUnload = false;
            for(int i = 0; i < containers.size(); ++i){
                if(containers[i].second == idxOfTopContainer.second){
                    isAboveContainerToUnload = true;
                    break;
                }
            }
            if(isAboveContainerToUnload){
                cont = move->grid[idxOfTopContainer.first][idxOfTopContainer.second];
                move = move_left(move, idxOfTopContainer);
                if(move == NULL){
                    children.push_back(make_pair(nullShip, new Container(cont->weight, cont->contents)));
                    break;
                }
            }
        }
        children.push_back(make_pair(new Ship(move, 0), nullCont));
        above--;
    }
    children.push_back(make_pair(nullShip, nullCont));
    for(int i = 0; i < children.size(); ++i){
        if(children.at(i).first != NULL){
            children.at(i).first->parent = this;
            children.at(i).first->setUniqueKey();
        }
    }
    return children;
}

int Ship::containersAbove(Ship* p, pair<int, int> idx){
    int cnt = 0;
    for(int i = 0; i < idx.first; ++i){
        if(p->grid[i][idx.second]->weight != -1){
            ++cnt;
        }
    }
    return cnt;
}

Ship* Ship::move_right(Ship *p, pair<int, int> &idx){
    if(idx.second + 1 >= 12){
        return NULL;
    }
    vector<vector<Container*>> g = p->grid;
    int row = idx.first;
    int column = idx.second + 1;

    for(int i = 0; i < g.size(); ++i){
        if(g[row][column]->weight == -1){
            if(row + 1 >= g.size()){
                swap(p->grid[row][column], p->grid[idx.first][idx.second]);
                idx.first = row;
                idx.second = column;
                return p;
            }
            else if(g[row + 1][column]->weight > 0){ //check to make sure about row+1 in OR, otherwise g[row+1] is out of bounds
                swap(p->grid[row][column], p->grid[idx.first][idx.second]);
                idx.first = row;
                idx.second = column;
                return p;
            }
            if(row < g.size()){row++;}
            while(row < p->grid.size()){ //moves crate to the very bottom or right before first occupied space in the column
                if(g[row][column]->weight > 0){
                    swap(p->grid[row-1][column], p->grid[idx.first][idx.second]);
                    idx.first = row-1;
                    idx.second = column;
                    return p;
                }
                if(row < g.size()){row++;}
                else{break;}
            }
        }
        if(row > 0){row--;}
        while(row > -1){ //edge case is if column is filled to the top
                if(g[row][column]->weight == -1){
                    swap(p->grid[row][column], p->grid[idx.first][idx.second]);
                    idx.first = row;
                    idx.second = column;
                    return p;
                }
                if(row > 0){row--;}
                else{break;}
        }
    }
    return p;
}

Ship* Ship::move_left(Ship *p, pair<int, int> &idx){
    if(idx.second - 1 < 0){
        return NULL;
    }
    vector<vector<Container*>> g = p->grid;
    int row = idx.first;
    int column = (idx.second > 0) ? idx.second - 1 : 0;

    for(int i = 0; i < idx.second; ++i){
        if(g[row][column]->weight == -1){
            if(row + 1 >= g.size()){
                swap(p->grid[row][column], p->grid[idx.first][idx.second]);
                idx.first = row;
                idx.second = column;
                return p;
            }
            else if(g[row + 1][column]->weight > 0){ 
                swap(p->grid[row][column], p->grid[idx.first][idx.second]);
                idx.first = row;
                idx.second = column;
                return p;
            }
            if(row < g.size()){row++;}
            while(row < p->grid.size()){ //move crate to bottom
                if(g[row][column]->weight > 0){
                    swap(p->grid[row-1][column], p->grid[idx.first][idx.second]);
                    idx.first = row-1;
                    idx.second = column;
                    return p;
                }
                if(row < g.size()){row++;}
                else{break;}
            }
        }
        if(row > 0){row--;}
        while(row > -1)
        {
                if(g[row][column]->weight == -1){
                    swap(p->grid[row][column], p->grid[idx.first][idx.second]);
                    idx.first = row;
                    idx.second = column;
                    return p;
                }
                if(row > 0){row--;}
                else{break;}
        }
    }
    return p;
}

vector<int> Ship::find_num_containers(){
    vector<int> temp = sort_larger_mass();
    string tmp = ret_larger_side();

    double def = deficit();
    double high_def = def/1.0 + (def * .1);
    double low_def = def/1.0 - (def * .1);
    vector<int> values;
    bool bal = false;
    int i = 0;

    double left = find_mass_left();
    double right = find_mass_right();
    double balance = 0;

    while(!(bal)){
        if((temp.at(i)/1.0 >= low_def && temp.at(i)/1.0 <= high_def) || temp.at(i)/1.0 <= def){
            low_def = low_def - temp.at(i);
            high_def = high_def - temp.at(i);
            def = def - temp.at(i);
            values.push_back(temp.at(i));
            if(tmp == "left"){
                left = left - temp.at(i);
            }
            if(tmp == "right"){
                right = right - temp.at(i);
            }

            if(tmp == "left"){
                right = right + temp.at(i);
            }
            if(tmp == "right"){
                left = left + temp.at(i);
            }

            balance = min(left,right)/max(left,right) ;
            if(balance >= .9){
                bal = true;
            }
        }
        if(i == temp.size()-1){
            bal = true;
        }
        i++;
    }

    values.push_back(values.size());
    return values;
}
