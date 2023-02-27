#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <string.h>
#include <fstream>
#include <windows.h>

using namespace std;

const int rows = 8, columns = 12;

const int defaultColor = 0x08;

struct container;
struct node;
class CompareNode;

void ShowConsoleCursor(bool);

void readManifest(const string&, vector<vector<container> >&);
void printShip(const vector<vector<container>>&, const vector<vector<container>>&);
void printChar(char, int, int);

void outputMoves(node*);
void outputMove(node*);

int left_mass(const vector<vector<container> >&);
int right_mass(const vector<vector<container> >&);
double deficit(const vector<vector<container> >&);
int get_hn(vector<vector<container> >&);
bool isGoalState(const vector<vector<container> >&, const vector<vector<container> >&);
bool isBufferEmpty(const vector<vector<container> >&);
node* general_search(vector<vector<container> >&);
void sift(vector<vector<container> >&);
vector<node*> expand(node*, priority_queue<node*, vector<node*>, CompareNode>&, map<string, bool>&);
pair<int,int> find_nearest_column(vector<vector<container> >&, int);
pair<int,int> findNearestBufferColumn(vector<vector<container> >&);
int top_container(vector<vector<container> >&, int);
int top_container_between(vector<vector<container> >&, int, int);
void a_star_search(priority_queue<node*, vector<node*>, CompareNode>&, vector<node*>&);
int balance_heuristic(vector<vector<container> >&, int);

struct container {
    int weight;
    string desc;
    container() : weight(0), desc("UNUSED") { };
    container(int w, string d) : weight(w), desc(d) { };
    container operator=(container c) {weight = c.weight; desc = c.desc; return c;}
};

struct node {
    vector<vector<container> > containers;
    vector<vector<container> > buffer;
    string animationMessage;
    node *parent = nullptr;
    vector<node*> children;
    int gn, hn;
    int cranePosY = 9;
    int cranePosX = 1;
    int totalTime = 0;
    node(vector<vector<container> > c) : containers(c) {
        for (int i = 0; i < 4; ++i) {
            buffer.push_back(vector<container>(24));
        }
    }
    node(node* n) {
        gn = n->gn; hn = n->hn;
        containers = n->containers;
        buffer = n->buffer;
        cranePosY = n->cranePosY;
        cranePosX = n->cranePosX;
        totalTime = n->totalTime;
        parent = nullptr;
        children.clear();
    }
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
};

// a larger f(n) is worse
class CompareNode {
 public:
    bool operator() (const node* lhs, const node* rhs) {
        return lhs->get_fn() > rhs->get_fn();
    }
};

HANDLE console_color = GetStdHandle(STD_OUTPUT_HANDLE);

int main() {

    ShowConsoleCursor(false);

    system("pause");

    SetConsoleTextAttribute(console_color, defaultColor);

    string manifest = "manifests\\ShipCase2.txt";

    vector<vector<container>> containers;
    for (int i = 0; i < rows; ++i) containers.push_back(vector<container>(columns));

    readManifest(manifest, containers);

    node* solution = general_search(containers);

    outputMoves(solution);

    system("pause");

    return 0;
}

void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

void readManifest(const string &manifest, vector<vector<container> >& containers) {
    ifstream fin(manifest);
    if (!fin.is_open()) {
        cout << "Error opening " << manifest << "!" << endl;
        exit(EXIT_FAILURE);
    }
    string coordinates, weight, description;
    int numContainers = 0;
    while (fin >> coordinates) {
        fin >> weight >> description;
        int y = (coordinates.at(1) - 0x30) * 10 + (coordinates.at(2) - 0x30);
        int x = (coordinates.at(4) - 0x30) * 10 + (coordinates.at(5) - 0x30);
        int w = stoi(weight.substr(1, 5));
        containers.at(y - 1).at(x - 1).weight = w;
        containers.at(y - 1).at(x - 1).desc = description;
        if (description != "NAN" && description != "UNUSED") ++numContainers;
    }
    //string message = "Manifest " + manifest + " is opened, there are " + to_string(numContainers) + " containers on the ship.";
    //log_File(logFile, message);
    fin.close();
}

void printShip(const vector<vector<container>>& containers, const vector<vector<container>>& buffer) {
    //[176]░ [177]▒ [178]▓ [219]█ [254]■
    string dsc;
    SetConsoleTextAttribute(console_color, 0x0c);
    cout << "                       " << char(254) << "    " << char(254) << "\n";
    SetConsoleTextAttribute(console_color, defaultColor);
    for (int y = rows; y >= 1; --y) {
        for (unsigned x = 1; x <= 24; ++x) {
            if (y <= 4) {
                printChar(219, 0x01, defaultColor);
            }
            else {
                dsc = buffer.at(y - 4 - 1).at(x - 1).desc;
                if (dsc == "UNUSED") cout << (char)176;
                else printChar(178, 0x04, defaultColor);
            }
        }
        cout << "    ";
        for (unsigned x = 1; x <= columns; ++x) {
            dsc = containers.at(y - 1).at(x - 1).desc;
            if (dsc == "NAN") printChar(219, 0x01, defaultColor);
            else if (dsc == "UNUSED") cout << (char)176;
            else printChar(178, 0x04, defaultColor);
        }
        cout << endl;
    }
    //for (int i = 0; i < columns + 2; ++i) cout << (char)219;
    cout << endl;
    return;
}

void printChar(char c, int new_color, int old_color) {
    SetConsoleTextAttribute(console_color, new_color);
    cout << c;
    SetConsoleTextAttribute(console_color, old_color);
}

void outputMoves(node* root) {
    node* n = root;
    while (n->children.size()) {
        outputMove(n);
        n = n->children.at(0);
    }
    outputMove(n);
    return;
}

void outputMove(node* n) {
    vector<vector<vector<container>>> containerFrames;
    vector<vector<vector<container>>> bufferFrames;
    string message = n->animationMessage;
    if (message == "") {
        system("CLS");
        cout << "Press Enter to begin Balance Operation\n\n";
        printShip(n->containers, n->buffer);
        while (!(GetAsyncKeyState(VK_RETURN) & 0x0001)) Sleep(200);
        return;
    }
    int startY = message.find('{') + 1;
    int startX = startY + 2;
    bool isStartTypeShip = message.at(startY - 3) == 'P';
    int endY = message.find('{', startY) + 1;
    int endX = endY + 2;
    bool isEndTypeShip = message.at(endY - 3) == 'P';
    startY = message.at(startY) - 0x30;
    startX = message.at(startX) - 0x30;
    endY = message.at(endY) - 0x30;
    endX = message.at(endX) - 0x30;
    vector<vector<container>> containers = n->containers;
    vector<vector<container>> buffer = n->buffer;
    if (isStartTypeShip && isEndTypeShip) {
        container temp = containers.at(endY - 1).at(endX - 1);
        containers.at(endY - 1).at(endX - 1) = containers.at(startY - 1).at(startX - 1);
        containers.at(startY - 1).at(startX - 1) = temp;
        containerFrames.push_back(containers);
        bufferFrames.push_back(buffer);
        int highestContainer = top_container_between(containers, startX + ((startX < endX) ? 1 : -1), endX);
        for (int y = startY; y <= highestContainer; ++y) {
            temp = containers.at(y - 1).at(startX - 1);
            containers.at(y - 1).at(startX - 1) = containers.at(y).at(startX - 1);
            containers.at(y).at(startX - 1) = temp;
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
        }
        if (startX < endX) {
            for (int x = startX; x < endX; ++x) {
                temp = containers.at(highestContainer).at(x - 1);
                containers.at(highestContainer).at(x - 1) = containers.at(highestContainer).at(x);
                containers.at(highestContainer).at(x) = temp;
                containerFrames.push_back(containers);
                bufferFrames.push_back(buffer);
            }
        }
        else {
            for (int x = startX; x > endX; --x) {
                temp = containers.at(highestContainer).at(x - 1);
                containers.at(highestContainer).at(x - 1) = containers.at(highestContainer).at(x - 2);
                containers.at(highestContainer).at(x - 2) = temp;
                containerFrames.push_back(containers);
                bufferFrames.push_back(buffer);
            }
        }
        for (int y = highestContainer + 1; y > endY; --y) {
            temp = containers.at(y - 1).at(endX - 1);
            containers.at(y - 1).at(endX - 1) = containers.at(y - 2).at(endX - 1);
            containers.at(y - 2).at(endX - 1) = temp;
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
        }
    }
    else if (!isStartTypeShip && isEndTypeShip) {
        container temp = containers.at(endY - 1).at(endX - 1);
        containers.at(endY - 1).at(endX - 1) = buffer.at(startY - 1).at(startX - 1);
        buffer.at(startY - 1).at(startX - 1) = temp;
        //CODE NOT COMPLETE
    }
    else if (isStartTypeShip && !isEndTypeShip) {
        container temp = buffer.at(endY - 1).at(endX - 1);
        buffer.at(endY - 1).at(endX - 1) = containers.at(startY - 1).at(startX - 1);
        containers.at(startY - 1).at(startX - 1) = temp;
        //CODE NOT COMPLETE
    }
    else if (!isStartTypeShip && !isEndTypeShip) {
        container temp = buffer.at(endY - 1).at(endX - 1);
        buffer.at(endY - 1).at(endX - 1) = containers.at(startY - 1).at(startX - 1);
        containers.at(startY - 1).at(startX - 1) = temp;
        //CODE NOT COMPLETE
    }
    bool interrupt = false;
    while (!interrupt) {
        for (int i = 0; i < containerFrames.size(); ++i) {
            if (GetAsyncKeyState(VK_RETURN) & 0x0001) {
                interrupt = true; break;
            }
            system("CLS");
            cout << message << "\n\n";
            printShip(containerFrames.at(i), bufferFrames.at(i));
            Sleep(200);
        }
        if (interrupt) break;
        for (int i = containerFrames.size() - 2; i >= 1; --i) {
            if (GetAsyncKeyState(VK_RETURN) & 0x0001) {
                interrupt = true; break;
            }
            system("CLS");
            cout << message << "\n\n";
            printShip(containerFrames.at(i), bufferFrames.at(i));
            Sleep(200);
        }
    }
    string successMessage = "Success! Moved " + message.substr(7);
    system("CLS");
    cout << successMessage << "\n\n";
    printShip(n->containers, n->buffer);
    while (!(GetAsyncKeyState(VK_RETURN) & 0x0001)) {
        Sleep(200);
    }
    return;
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

bool isGoalState(const vector<vector<container> >& containers, const vector<vector<container> >& buffer) {
    if (!isBufferEmpty(buffer)) return false;
	if (left_mass(containers) > right_mass(containers)) {
		return (deficit(containers) <= left_mass(containers) * 0.1);
    }
	return (deficit(containers) <= right_mass(containers) * 0.1);
}

bool isBufferEmpty(const vector<vector<container> >& buffer) {
    for (int y = 1; y <= 4; ++y) {
        for (int x = 1; x < 24; ++x) {
            if (buffer.at(y - 1).at(x - 1).desc != "UNUSED") {
                return false;
            }
        }
    }
    return true;
}

void sift(vector<vector<container> >& containers) {

}


node* general_search(vector<vector<container> >& containers) {
    priority_queue<node*, vector<node*>, CompareNode> nodes;
    node *initial_state = new node(containers);
    initial_state->set_gn(0);
    initial_state->set_hn(balance_heuristic(initial_state->containers, initial_state->totalTime));
    nodes.push(initial_state);
    unsigned max_queue_size = 1;
    unsigned nodes_expanded = 0;
    map<string, bool> explored_states;
    while(true) {
        if (nodes.empty()) {
            cout << "Ship could not be balanced. Beginning SIFT operation." << endl;
            sift(containers);
            cout << "SIFT operation complete" << endl;
            return nullptr;
        }
        //if (nodes.size() > max_queue_size) {
        //    max_queue_size = nodes.size();
        //}
        node* curr_state = nodes.top();
        nodes.pop();
        printShip(curr_state->containers, curr_state->buffer); //DEBUG REMOVE LATER!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //cout << "The best state to expand with a g(n) = " << curr_state.get_gn() << " and h(n) = " << curr_state.get_hn() << " is..." << endl;
        if (isGoalState(curr_state->containers, curr_state->buffer)) {
            cout << "\nGoal state!\n" << endl;
            //cout << "Solution depth was " << curr_state.get_gn() << endl;
            //cout << "Number of nodes expanded: " << nodes_expanded << endl;
            //cout << "Max queue size: " << max_queue_size << endl;
            node* temp = curr_state;
            node* tempChild = nullptr;
            while (temp->parent) {
                tempChild = temp;
                temp = temp->parent;
                for (vector<node*>::reverse_iterator it = temp->children.rbegin(); it != temp->children.rend(); ++it) {
                    if (*it != tempChild) {
                        delete *it;
                        temp->children.erase((it+1).base());
                    }
                }
            }
            return temp;
        }
        vector<node*> new_nodes = expand(curr_state, nodes, explored_states);
        a_star_search(nodes, new_nodes);
        ++nodes_expanded;
    }
}

vector<node*> expand(node* curr_state, priority_queue<node*, vector<node*>, CompareNode>& nodes, map<string, bool>& explored_states) {
    vector<node*> new_nodes;
    explored_states[curr_state->to_string()] = true;
    int right_heavier = 6 * (right_mass(curr_state->containers) > left_mass(curr_state->containers));
    for (int i = 1 + right_heavier; i < 7 + right_heavier; ++i)
    {
        node *new_node = new node(curr_state);
        if (top_container(new_node->containers, i) == 0) continue; //no container in column
        //
        int highest_container_crane_pass = 0;
        int lower = min(i, new_node->cranePosX);
        int upper = max(i, new_node->cranePosX);
        for (int j = lower; j <= upper; ++j) {
            if (top_container(new_node->containers, j) > highest_container_crane_pass) {
               highest_container_crane_pass = top_container(new_node->containers, j);
            }
        }
        int curr_cell_row = top_container(new_node->containers,i);
        new_node->animationMessage = "Moving SHIP {" + to_string(curr_cell_row) + "," + to_string(i) + "} " + new_node->containers.at(curr_cell_row - 1).at(i - 1).desc + " to ";   
        new_node->totalTime += (abs(highest_container_crane_pass - new_node->cranePosY + 1)) + abs(new_node->cranePosX - i) + (highest_container_crane_pass - curr_cell_row);
        //
        pair<int,int> p = find_nearest_column(new_node->containers,i);
        int closest_cell_column = p.first;
        if(closest_cell_column == -1) {
            pair<int, int> bp = findNearestBufferColumn(new_node->buffer);
            closest_cell_column = bp.first;
            int closestBufferRow;
            for(int j = 1; j <= 4; ++j){
                if(new_node->buffer.at(j - 1).at(closest_cell_column - 1).desc == "UNUSED"){
                    closestBufferRow = j;
                    break;
                }
            }
            new_node->totalTime += bp.second;
            container temp = new_node->containers.at(i).at(curr_cell_row);
            new_node->containers.at(i).at(curr_cell_row) = new_node->buffer.at(closestBufferRow).at(closest_cell_column);
            new_node->buffer.at(closestBufferRow).at(closest_cell_column) = temp;
            new_node->cranePosY = -1 * closestBufferRow;
	        new_node->cranePosX = -1 * closest_cell_column;
            new_node->animationMessage += "BUFFER {" + to_string(closestBufferRow) + "," + to_string(closest_cell_column) + "}";
        }
        else {
            int closest_cell_row = top_container(new_node->containers,closest_cell_column) + 1;
            new_node->totalTime += p.second;
            container temp = new_node->containers.at(curr_cell_row - 1).at(i - 1);
            new_node->containers.at(curr_cell_row - 1).at(i - 1) = new_node->containers.at(closest_cell_row - 1).at(closest_cell_column - 1);
            new_node->containers.at(closest_cell_row - 1).at(closest_cell_column - 1) = temp;
            new_node->cranePosY = closest_cell_row;
	        new_node->cranePosX = closest_cell_column;
            new_node->animationMessage += "SHIP {" + to_string(closest_cell_row) + "," + to_string(closest_cell_column) + "}";
        }
        if (explored_states[new_node->to_string()] == false) {
            new_nodes.push_back(new_node);
        }
    }
    for(int i = 0; i < new_nodes.size(); ++i) {
        new_nodes.at(i)->parent = curr_state;
    }
    curr_state->children = new_nodes;
    return new_nodes;
}


pair<int,int> find_nearest_column(vector<vector<container> >& containers, int current_column) {
	bool right_heavier = current_column > 6;
	int lowestTime = INT_MAX;
	int nearestColumn = -1;
	if (right_heavier) {
	    for(int i = 6; i >= 1; --i) {
		    int highest_container = 0;
            if (top_container(containers, i) == rows) continue; //cannot put container in full column
		    for (int j = current_column; j >= i; --j) {
			    if (top_container(containers, j) > highest_container) {
				    highest_container = top_container(containers, j);
                }
		    }
		    int currTime = (highest_container - top_container(containers, current_column) + 1) + (i - current_column) + (highest_container - top_container(containers, i));
		    if(currTime < lowestTime) {
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
		    if(currTime < lowestTime) {
			    nearestColumn = i;
			    lowestTime = currTime;
		    }
	    }
    }
    return pair<int,int>(nearestColumn, lowestTime);
}

pair<int,int> findNearestBufferColumn(vector<vector<container> >& buffer) {
    int nearestColumn = -1;
    int row = -1;
    int totalTime = INT_MAX;
    for (int i = 24; i >= 1; --i) {
        for (int j = 1; j <= 4; ++j) {
            if (buffer.at(j - 1).at(i - 1).desc == "UNUSED") {
                nearestColumn = i;
                row = j;
                break;
            }
        }
        if (nearestColumn != -1) {
            break;
        }
    }
    totalTime = 4 + ((24 - nearestColumn) + (5 - row));
    return pair<int,int>(nearestColumn, totalTime);
}

int top_container(vector<vector<container> >& containers, int column) {
    for (int y = 8; y >= 1; --y) {
        string desc = containers.at(y - 1).at(column - 1).desc;
        if (desc != "UNUSED") {
            return y;
        }
    }
    return 0;
}

int top_container_between(vector<vector<container> >& containers, int column1, int column2) {
    int highest_container = 0;
    int lower = min(column1, column2);
    int upper = max(column1, column2);
    for (int i = lower; i <= upper; ++i) {
        if (top_container(containers, i) > highest_container) {
            highest_container = top_container(containers, i);
        }
    }
    return highest_container;
}

void a_star_search(priority_queue<node*, vector<node*>, CompareNode>& nodes, vector<node*>& new_nodes) {
    for (unsigned i = 0; i < new_nodes.size(); ++i) {
        new_nodes[i]->set_gn(new_nodes[i]->totalTime);
        new_nodes[i]->set_hn(balance_heuristic(new_nodes[i]->containers, new_nodes[i]->totalTime));
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
