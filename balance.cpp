#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <string.h>


using namespace std;

const int rows = 8, columns = 12;

struct container;
struct node;

int left_mass(const vector<vector<container> >&);
int right_mass(const vector<vector<container> >&);
double deficit(const vector<vector<container> >&);
int get_hn(vector<vector<container> >&);
bool isGoalState(const vector<vector<container> >&);
void general_search(vector<vector<container> >&);
void sift(vector<vector<container> >&);
vector<node> expand(node&, priority_queue<node>&, map<string, bool>&);
pair<int,int> find_nearest_column(vector<vector<container> >&, int);
int top_container(vector<vector<container> >&, int);
void a_star_search(priority_queue<node>&, vector<node>&);
int balance_heuristic(vector<vector<container> >&, int);


struct container {
    int y;
    int x;
    int weight;
    string desc;
    container(int y, int x, int w, string d) : y(y), x(x), weight(w), desc(d) { };
    container operator=(container c) {y = c.y; x = c.x; weight = c.weight; desc = c.desc; return c;}
};

struct node {
    vector<vector<container> > containers;
    int gn, hn;
    int cranePosY = 9;
    int cranePosX = 1;
    int totalTime = 0;
    node(vector<vector<container> > c) : containers(c) { }
    int get_fn() const {return gn + hn;} //estimated cost of the cheapest solution that goes through node n
    int get_gn() const {return gn;} //the cost to get to a node
    int get_hn() const {return hn;} //the estimated distance to the goal
    void set_gn(int g) {gn = g;}
    void set_hn(int h) {hn = h;}
    string to_string() {
        string n;
        for (const auto& row : containers)
            for (const auto& elem : row)
                n += (std::to_string(elem.weight) + ",");
        return n;
    }
    node operator=(node n) {
        gn = n.gn; hn = n.hn;
        containers = n.containers;
        cranePosY = n.cranePosY;
        cranePosX = n.cranePosX;
        totalTime = n.totalTime;
        return n;
    }
};

// a larger f(n) is worse
bool operator<(const node& lhs, const node& rhs) {
    return lhs.get_fn() > rhs.get_fn();
}

int main() {
    /*
    container a(1, 1, 98, "a");
    container b(1, 2, 1, "b");
    container c(1, rows, 91, "c");
    vector<vector<container>> containers;
    containers.push_back(a);
    containers.push_back(b);
    containers.push_back(c);
    cout << "The left side has a combined mass of " << left_mass(containers) << " kgs." << endl;
    cout << "The right side has a combined mass of " << right_mass(containers) << " kgs." << endl;
    */
    return 0;
}

int left_mass(const vector<vector<container> >& containers) {
    int mass = 0;
    for (int i = 0; i < rows; ++i) {
            for(int j = 0; j < columns/2; j++){
                mass += containers.at(i).at(j).weight;
            }
    }
    return mass;
}

int right_mass(const vector<vector<container> >& containers) {
    int mass = 0;
     for (int i = 0; i < rows; ++i) {
            for(int j = columns/2; j < columns; j++){
                mass += containers.at(i).at(j).weight;
            }
    }
    return mass;
}

double deficit(const vector<vector<container> >& containers) {
    return abs(left_mass(containers) - right_mass(containers)) / 2.0;
}

bool isGoalState(const vector<vector<container> >& containers){
	if(left_mass(containers) > right_mass(containers))
		return (deficit(containers) <= left_mass(containers) * 0.1);
	return (deficit(containers) <= right_mass(containers) * 0.1);
}

void sift(vector<vector<container> >& containers) { }


void general_search(vector<vector<container> >& containers) {
    priority_queue<node> nodes;
    node initial_state(containers);
    initial_state.set_gn(0);
    initial_state.set_hn(balance_heuristic(initial_state.containers, initial_state.totalTime));
    nodes.push(initial_state);
    unsigned max_queue_size = 1;
    unsigned nodes_expanded = 0;
    map<string, bool> explored_states;
    while(true) {
        if (nodes.empty()) {
            cout << "Ship could not be balanced. Beginning SIFT operation." << endl;
            sift(containers);
        }
        //if (nodes.size() > max_queue_size) {
        //    max_queue_size = nodes.size();
        //}
        node curr_state = nodes.top();
        nodes.pop();
        //cout << "The best state to expand with a g(n) = " << curr_state.get_gn() << " and h(n) = " << curr_state.get_hn() << " is..." << endl;
        if (isGoalState(containers)) {
            cout << "\nGoal state!\n" << endl;
            //cout << "Solution depth was " << curr_state.get_gn() << endl;
            //cout << "Number of nodes expanded: " << nodes_expanded << endl;
           // cout << "Max queue size: " << max_queue_size << endl;
            return;
        }
        vector<node> new_nodes = expand(curr_state, nodes, explored_states);
        a_star_search(nodes, new_nodes);
        ++nodes_expanded;
    }
}

vector<node> expand(node& curr_state, priority_queue<node>& nodes, map<string, bool>& explored_states) {
    int row = curr_state.cranePosY;
    int column = curr_state.cranePosX;
    vector<node> new_nodes;
    explored_states[curr_state.to_string()] = true;
    int right_heavier = 6 * (right_mass(curr_state.containers) > left_mass(curr_state.containers));
    for (int i = 1 + right_heavier; i < 7 + right_heavier; ++i)
    {
        node new_node = curr_state;
        if (top_container(new_node.containers, i) == 0) continue; //no container in column
        pair<int,int> p = find_nearest_column(new_node.containers,i);
        int closest_cell_column = p.first;
        int closest_cell_row = top_container(new_node.containers,closest_cell_column);
        //
        int highest_container_crane_pass = 0;
        int lower = min(closest_cell_column, new_node.cranePosX);
        int upper = max(closest_cell_column, new_node.cranePosX);
        for (int j = lower; j <= upper; ++j) {
            if (top_container(new_node.containers, j) > highest_container_crane_pass) {
               highest_container_crane_pass = top_container(new_node.containers, j);
            }
        }
        new_node.totalTime += (highest_container_crane_pass - new_node.cranePosY + 1) + (upper - lower) + (highest_container_crane_pass - closest_cell_row);
        //
        new_node.totalTime += p.second;
        int curr_cell_row = top_container(new_node.containers,i); 
        container temp = new_node.containers.at(i).at(curr_cell_row);
        new_node.containers.at(i).at(curr_cell_row) = new_node.containers.at(closest_cell_row).at(closest_cell_column);
        new_node.containers.at(closest_cell_row).at(closest_cell_column) = temp;
        new_node.cranePosY = closest_cell_row;
	    new_node.cranePosX = closest_cell_column;

        if (explored_states[new_node.to_string()] == false) {
            new_nodes.push_back(new_node);
        }
    }
    return new_nodes;
}


pair<int,int> find_nearest_column(vector<vector<container> >& containers, int current_column) {
	bool right_heavier = current_column > 6;
	int lowestTime = INT_MAX;
	int nearestColumn;
	if (right_heavier) {
	    for(int i = 6; i >= 1; --i){
		    int highest_container = 0;
            if (top_container(containers, i) == rows) continue; //cannot put container in full column
		    for (int j = current_column; j >= i; --j) {
			    if (top_container(containers, j) > highest_container) {
				    highest_container = top_container(containers, j);
                }
		    }
		    int currTime = (highest_container - top_container(containers, current_column) + 1) + (i - current_column) + (highest_container - top_container(containers, i));
		    if(currTime < lowestTime){
    			nearestColumn = i;
	    		lowestTime = currTime;
		    }
        }
    }
	else {
		for(int i = 7; i <= 12; ++i){
		    int highest_container = 0;
            if (top_container(containers, i) == rows) continue; //cannot put container in full column
		    for (int j = current_column; j <= i; ++j) {
			    if (top_container(containers, j) > highest_container) {
				    highest_container = top_container(containers, j);
                }
		    }
		    int currTime = (highest_container - top_container(containers, current_column) + 1) + (i - current_column) + (highest_container - top_container(containers, i));
		    if(currTime < lowestTime){
			    nearestColumn = i;
			    lowestTime = currTime;
		    }
	    }
    }
    return pair<int,int>(nearestColumn, lowestTime);
}


int top_container(vector<vector<container> >& containers, int column) {
    for (int y = 8; y >= 1; --y) {
        string desc = containers.at(y - 1).at(column - 1).desc;
        if (desc != "NAN" && desc != "UNUSED") {
            return y;
        }
    }
    return 0;
}


void a_star_search(priority_queue<node>& nodes, vector<node>& new_nodes) {
    for (unsigned i = 0; i < new_nodes.size(); ++i) {
        new_nodes[i].set_gn(new_nodes[i].get_gn() + 1);
        new_nodes[i].set_hn(balance_heuristic(new_nodes[i].containers, new_nodes[i].totalTime));
        nodes.push(new_nodes[i]);
    }
}

int balance_heuristic(vector<vector<container> >& containers, int totalTime) {
    int heavier_side_weight = left_mass(containers);
    int lighter_side_weight = right_mass(containers);
    if (lighter_side_weight > heavier_side_weight) {
        swap(lighter_side_weight, heavier_side_weight);
    }
    int hn_weight = heavier_side_weight * 0.9 - lighter_side_weight;
    return hn_weight * 0.5 + totalTime * 0.5;
}
