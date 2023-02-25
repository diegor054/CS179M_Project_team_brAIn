#include <iostream>
#include <vector>
#include <map>
#include <queue>


using namespace std;

const int rows = 8, columns = 12;

struct container;

int left_mass(const vector<vector<container>>&);
int right_mass(const vector<vector<container>>&);
double deficit(const vector<vector<container>>&);
int get_hn(vector<vector<container>>&);
bool isGoalState(const vector<vector<container>>&);
void general_search(vector<vector<container>>&);
void sift(vector<vector<container>>&);


struct container {
    int y;
    int x;
    int weight;
    string desc;
    container(int y, int x, int w, string d) : y(y), x(x), weight(w), desc(d) { };
    container operator=(container c) {y = c.y; x = c.x; weight = c.weight; desc = c.desc; return c;}
};

struct node {
    vector<vector<container>> containers;
    int fn, gn, hn;
    node(vector<vector<container>> c, int g) : containers(c), gn(g) {init();}
    node(node &n) : containers(n.containers), gn(n.gn + 1) {init();}
    void init() {hn = get_hn(containers); fn = gn + hn;}
    container getContainer(int y, int x) const {return containers.at((y - 1) * columns + (x - 1));}
    void setContainer(int y, int x, container& c) {containers.at((y - 1) * columns + (x - 1)) = c;}
};

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

int left_mass(const vector<vector<container>>& containers) {
    int mass = 0;
    for (int i = 0; i < rows; ++i) {
            for(int j = 0; j < columns/2; j++){
                mass += containers.at(i).at(j).weight;
            }
    }
    return mass;
}

int right_mass(const vector<vector<container>>& containers) {
    int mass = 0;
     for (int i = 0; i < rows; ++i) {
            for(int j = columns/2; j < columns; j++){
                mass += containers.at(i).at(j).weight;
            }
    }
    return mass;
}

double deficit(const vector<container>& containers) {
    return abs(left_mass(containers) - right_mass(containers)) / 2.0;
}

bool isGoalState(const vector<vector<container>>& containers){
	if(left_mass(containers) > right_mass(containers))
		return (deficit(containers) <= left_mass(containers) * 0.1);
	return (deficit(containers) <= right_mass(containers) * 0.1);
}

void sift(vector<vector<container>>& containers) { }


void general_search(vector<vector<container>>& containers) {
    priority_queue<node> nodes;
    node initial_state(containers, 0);
    //initial_state.set_gn(0);
    //initial_state.set_hn(0);
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
        a_star_search(nodes, new_nodes, heuristic);
        ++nodes_expanded;
    }
}

