#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <string.h>
#include <string>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <windows.h>

using namespace std;

const int rows = 8, columns = 12;

const int defaultColor = 0x08;

ofstream logFile;

string userName;

struct container;
struct node;
class CompareNode;

void ShowConsoleCursor(bool);

void readManifest(const string&, vector<vector<container> >&);
void writeManifest(const string &, vector<vector<container>> &);
void logIn();
bool menu();
void menuScreen();
void writeComment();
void log(const string &);
void printShip(const vector<vector<container>>&, const vector<vector<container>>&, int);
void printChar(char, int, int);
void printString(string&, int, int);
int totalMoves(node*);
void outputMoves(pair<node*, int>);
void outputMove(node*, int, int, int);

int left_mass(const vector<vector<container> >&);
int right_mass(const vector<vector<container> >&);
double deficit(const vector<vector<container> >&);
int get_hn(vector<vector<container> >&);
bool isGoalState(const vector<vector<container> >&, const vector<vector<container> >&);
bool isBufferEmpty(const vector<vector<container> >&);
pair<node*, int> general_search(vector<vector<container> >&);
pair<node*, int> sift(vector<vector<container> >&);
vector<vector<container>> siftGoalState(vector<vector<container> >&);
bool isSiftGoalState(const vector<vector<container> >&, const vector<vector<container> >&, const vector<vector<container> >&);
void sift_a_star_search(priority_queue<node*, vector<node*>, CompareNode>&, vector<node*>&, vector<vector<container>> &);
int sift_Heuristic(vector<vector<container>> &, vector<vector<container>> &, vector<vector<container>> &);
pair<int, int> findContainer(container &, vector<vector<container>> &);
vector<node*> expand(node*, priority_queue<node*, vector<node*>, CompareNode>&, map<string, bool>&);
pair<int,int> find_nearest_column(vector<vector<container> >&, int);
pair<int,int> findNearestBufferColumn(vector<vector<container> >&);
int top_container(vector<vector<container> >&, int);
int top_container_buffer(vector<vector<container> >& , int);
int top_container_between(vector<vector<container> >&, int, int);
int top_container_buffer_between(vector<vector<container> >&, int, int);
void a_star_search(priority_queue<node*, vector<node*>, CompareNode>&, vector<node*>&);
int balance_heuristic(vector<vector<container> >&, vector<vector<container> >&);


struct container {
    int weight;
    string desc;
    container() : weight(0), desc("UNUSED") { };
    container(int w, string d) : weight(w), desc(d) { };
    container operator=(container c) {weight = c.weight; desc = c.desc; return c;}
};

bool operator<(const container &lhs, const container &rhs){
    return lhs.weight < rhs.weight;
}

bool operator!=(const container &lhs, const container &rhs){
    return ((lhs.weight != rhs.weight) || (lhs.desc != rhs.desc)); 
}

bool operator==(const container &lhs, const container &rhs){
    return ((lhs.weight == rhs.weight) && (lhs.desc == rhs.desc)); 
}

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
        animationMessage = n->animationMessage;
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
        string prev = "\0", curr;
        int count = 1;
        for (const auto& row : containers) {
            for (const auto& elem : row) {
                curr = std::to_string(elem.weight);
                if (curr != prev) {
                    if (count > 1) {
                        n += "." + std::to_string(count);
                    }
                    if (prev != "\0") {
                        n += ",";
                    }
                    n += curr;
                    count = 1;
                }
                else {
                    ++count;
                }
                prev = curr;
            }
        }
        if (count > 1) {
            n += "." + std::to_string(count);
        }
        count = 1;
        prev = "\0";
        for (const auto& row : buffer) {
            for (const auto& elem : row) {
                curr = std::to_string(elem.weight);
                if (curr != prev) {
                    if (count > 1) {
                        n += "." + std::to_string(count);
                    }
                    n += ",";
                    n += curr;
                    count = 1;
                }
                else {
                    ++count;
                }
                prev = curr;
            }
        }
        if (count > 1) {
            n += "." + std::to_string(count);
        }
        //cout << n << '\n';
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

    SetConsoleTextAttribute(console_color, defaultColor);

    ifstream logCopy;
    logCopy.open("log.txt");
    string previousLog, temp;
    while(getline(logCopy, temp)) previousLog += temp + '\n';
    logCopy.close();
    logFile.open("log.txt");
    logFile << previousLog;

    logIn();

    logFile.close();
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
    string message = "Manifest " + manifest + " is opened, there are " + to_string(numContainers) + " containers on the ship.";
    log(message);
    fin.close();
}

void writeManifest(const string &manifest, vector<vector<container>> &containers){
    string updatedManifest = manifest.substr(0, manifest.size() - 4) + "OUTBOUND.txt";
    ofstream fout(updatedManifest);
    if (!fout.is_open()) {
        cout << "Error opening " << manifest << "!" << endl;
        exit(EXIT_FAILURE);
    }

    for(int i = 1; i <= rows; ++i){
        for(int j = 1; j <= columns; ++j){
            fout << "[" << setw(2) << setfill('0') << i << "," << setw(2) << setfill('0') << j << "], ";
            fout << "{" << setw(5) << setfill('0') << containers.at(i - 1).at(j - 1).weight << "}, ";
            fout << containers.at(i - 1).at(j - 1).desc << endl;
        }
    }
    
    string message = "Finished a Cycle. Manifest " + updatedManifest + " was written to desktop, and a reminder pop-up to operator to send file was displayed.";
    log(message);
    fout.close();
}

void logIn(){
    string name;
    do{
        do{
            system("cls");
            cout << "Welcome! Please enter your name to log in. \n\nUsername: " << flush;
            getline(cin, name);
        }while(name == "");

        string logInMessage = name +  " logged in.";
        log(logInMessage);
        userName = name;
    } while(menu());
}

bool menu(){
    menuScreen();
    char optionChosen = ' ';
    while(optionChosen == ' '){
        if(GetAsyncKeyState('S') & 0x8000){
            FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
            return true;
        }else if(GetAsyncKeyState('L') & 0x8000){
            cout << "Unload and Load functionality is not available." << endl;
        }else if(GetAsyncKeyState('B') & 0x8000){
            FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
            cout << "Enter Manifest File Name" << endl;
            string manifest;
            getline(cin, manifest);
            manifest = "manifests\\" + manifest;
            vector<vector<container>> containers;
            for (int i = 0; i < rows; ++i) containers.push_back(vector<container>(columns));
            readManifest(manifest, containers);
            pair<node*, int> solution = general_search(containers);
            outputMoves(solution);
            while(solution.first->children.size()) solution.first = solution.first->children.at(0);
            writeManifest(manifest, solution.first->containers);
            cout << "Reminder: Email the updated Manifest to the ship captain." << endl;;
            system("pause");
            menuScreen();
        }else if(GetAsyncKeyState('C') & 0x8000){
            writeComment();
            menuScreen();
        }else if(GetAsyncKeyState('E') & 0x8000){
            return false;
        }
        Sleep(100);
    }

    return false;
}

void menuScreen(){
    //[187]╗ [188]╝ [186]║ [200]╚ [205]═ [201]╔
    system("cls");
    cout << (char)201;
    for (int i = 0; i < 38; ++i) {
        cout << (char)205;
    }
    cout << (char)187 << '\n';

    string line1, line2, line3, line4, line5, blank;
    blank = "                                      ";
    line1 = "        [S] Switch User               ";
    line2 = "        [L] Load/Unload Ship          ";
    line3 = "        [B] Balance Ship              ";
    line4 = "        [C] Write Comment To Log      ";
    line5 = "        [E] Exit                      ";
    vector<string> lines = {blank, blank, line1, blank, blank, line2, blank, blank, line3, blank, blank, line4, blank, blank, line5, blank, blank};

    for(int i = 0; i < lines.size(); ++i){
        cout << (char)186 << lines.at(i) << (char)186 << '\n';
    }

    cout << (char)200;
    for (int i = 0; i < 38; ++i) {
        cout << (char)205;
    }
    cout << (char)188 << '\n';
}

void writeComment(){
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    cout << "Enter Comment:" << endl;
    string comment;
    while(comment == "") getline(cin, comment);
    log(comment);
}

void log(const string &message){
	time_t curr_time;
	tm * curr_tm;
	char date_[100];
    char date2_[100];
	char time_[100];
	
	time(&curr_time);
	curr_tm = localtime(&curr_time);
    string dayType;
    int day = curr_tm->tm_mday;
    switch(day%10){
        case 1:
        dayType = "st";
        break;
        case 2:
        dayType = "nd";
        break;

        case 3: 
        dayType= "rd";
        break;

        default:
        dayType = "th";
        break;
    }
    strftime(date_, 50, "%B %d", curr_tm);
    strftime(date2_, 50, " %Y:", curr_tm);
	strftime(time_, 50, "%H:%M", curr_tm);
	
	logFile << "[" << date_;
    logFile << dayType << date2_ << " ";
	logFile << time_ << "] ";
    logFile << message << "\n";
}


void printShip(const vector<vector<container>>& containers, const vector<vector<container>>& buffer, int outsideContainerColumn) {
    //[176]░ [177]▒ [178]▓ [219]█ [254]■
    string bufferRow5 = "                       ", shipRow9 = "           \n";
    if (outsideContainerColumn < 0 && outsideContainerColumn != -24) {
        bufferRow5.at(abs(outsideContainerColumn) - 1) = char(178);
    }
    if (outsideContainerColumn > 1) {
        shipRow9.at(outsideContainerColumn - 1 - 1) = char(178);
    }
    printString(bufferRow5, 0x04, defaultColor);
    if (outsideContainerColumn == -24) printChar(178, 0x04, defaultColor);
    else printChar(254, 0x0c, defaultColor);
    cout << "    ";
    if (outsideContainerColumn == 1) printChar(178, 0x04, defaultColor);
    else printChar(254, 0x0c, defaultColor);
    printString(shipRow9, 0x04, defaultColor);
    string dsc;
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
    cout << endl;
    return;
}

void printChar(char c, int new_color, int old_color) {
    SetConsoleTextAttribute(console_color, new_color);
    cout << c;
    SetConsoleTextAttribute(console_color, old_color);
}

void printString(string& s, int new_color, int old_color) {
    SetConsoleTextAttribute(console_color, new_color);
    cout << s;
    SetConsoleTextAttribute(console_color, old_color);
}

int totalMoves(node* root){
    int moves = 0;
    node* n = root;
    while(n->children.size()){
        n = n->children.at(0);
        ++moves;
    }
    return moves;
}

void outputMoves(pair<node*, int> root) {
    node* n = root.first;
    int currentMove = 0;
    int totalNumberMoves = totalMoves(n);
    while (n->children.size()) {
        outputMove(n, root.second, currentMove, totalNumberMoves);
        n = n->children.at(0);
        ++currentMove;
    }
    outputMove(n, root.second, currentMove, totalNumberMoves);
    return;
}

void outputMove(node* n, int totalTime, int currentMoves, int numMoves) {
    vector<vector<vector<container>>> containerFrames;
    vector<vector<vector<container>>> bufferFrames;
    vector<int> topRowContainerColumns;
    string message = n->animationMessage;
    if (message == "") {
        system("CLS");
        cout << "Press Enter to begin Balance Operation\n\n";
        printShip(n->containers, n->buffer, 0);
        cout << "Step " << currentMoves << "/" << numMoves << " completed." << endl;
        cout << "Remaining time: " << (totalTime - n->totalTime) << " minutes" << endl;
        while (!(GetAsyncKeyState(VK_RETURN) & 0x8000)) {
            if(GetAsyncKeyState('C') & 0x8000){
                writeComment();
            }
            Sleep(100);
        }
        return;
    }
    int startY = message.find('{') + 1;
    int startX = startY + 2;
    bool isStartTypeShip = message.at(startY - 3) == 'P';
    int endY = message.find('{', startY) + 1;
    int endX = endY + 2;
    bool isEndTypeShip = message.at(endY - 3) == 'P';
    startY = message.at(startY) - 0x30;
    if(isdigit(message.at(startX + 1))){
        startX = (message.at(startX) - 0x30) * 10 + (message.at(startX + 1) - 0x30);
    }else{
        startX = message.at(startX) - 0x30;
    }
    endY = message.at(endY) - 0x30;
    if(isdigit(message.at(endX + 1))){
        endX = (message.at(endX) - 0x30) * 10 + (message.at(endX + 1) - 0x30);
    }else{
        endX = message.at(endX) - 0x30;
    }
    vector<vector<container>> containers = n->containers;
    vector<vector<container>> buffer = n->buffer;
    if (isStartTypeShip && isEndTypeShip) {
        container temp = containers.at(endY - 1).at(endX - 1);
        containers.at(endY - 1).at(endX - 1) = containers.at(startY - 1).at(startX - 1);
        containers.at(startY - 1).at(startX - 1) = temp;
        containerFrames.push_back(containers);
        bufferFrames.push_back(buffer);
        topRowContainerColumns.push_back(0);
        int highestContainer = top_container_between(containers, startX + ((startX < endX) ? 1 : -1), endX);
        bool useTopRow = (highestContainer == 8);
        highestContainer -= useTopRow;
        for (int y = startY; y <= highestContainer; ++y) {
            temp = containers.at(y - 1).at(startX - 1);
            containers.at(y - 1).at(startX - 1) = containers.at(y).at(startX - 1);
            containers.at(y).at(startX - 1) = temp;
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(0);
        }
        int currContainerRow = max(startY, highestContainer + 1);
        if (useTopRow) {
            temp = containers.at(8 - 1).at(startX - 1);
            containers.at(8 - 1).at(startX - 1) = container();
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(startX);
        }
        if (startX < endX) {
            for (int x = startX; x < endX; ++x) {
                if (!useTopRow) {
                    temp = containers.at(currContainerRow - 1).at(x - 1);
                    containers.at(currContainerRow - 1).at(x - 1) = containers.at(currContainerRow - 1).at(x);
                    containers.at(currContainerRow - 1).at(x) = temp;
                }
                containerFrames.push_back(containers);
                bufferFrames.push_back(buffer);
                topRowContainerColumns.push_back(0 + (x + 1) * useTopRow);
            }
        }
        else {
            for (int x = startX; x > endX; --x) {
                if (!useTopRow) {
                    temp = containers.at(currContainerRow - 1).at(x - 1);
                    containers.at(currContainerRow - 1).at(x - 1) = containers.at(currContainerRow - 1).at(x - 2);
                    containers.at(currContainerRow - 1).at(x - 2) = temp;
                }
                containerFrames.push_back(containers);
                bufferFrames.push_back(buffer);
                topRowContainerColumns.push_back(0 + (x - 1) * useTopRow);
            }
        }
        if (useTopRow) {
            containers.at(8 - 1).at(endX - 1) = temp;
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(0);
        }
        for (int y = currContainerRow; y > endY; --y) {
            temp = containers.at(y - 1).at(endX - 1);
            containers.at(y - 1).at(endX - 1) = containers.at(y - 2).at(endX - 1);
            containers.at(y - 2).at(endX - 1) = temp;
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(0);
        }
    }
    else if (!isStartTypeShip && isEndTypeShip) {
        container temp = containers.at(endY - 1).at(endX - 1);
        containers.at(endY - 1).at(endX - 1) = buffer.at(startY - 1).at(startX - 1);
        buffer.at(startY - 1).at(startX - 1) = temp;
        containerFrames.push_back(containers);
        bufferFrames.push_back(buffer);
        topRowContainerColumns.push_back(0);
        for (int y = startY; y < 4; ++y) {
            temp = buffer.at(y - 1).at(startX - 1);
            buffer.at(y - 1).at(startX - 1) = buffer.at(y).at(startX - 1);
            buffer.at(y).at(startX - 1) = temp;
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(0);
        }
        temp = buffer.at(4 - 1).at(startX - 1);
        buffer.at(4 - 1).at(startX - 1) = container();
        containerFrames.push_back(containers);
        bufferFrames.push_back(buffer);
        topRowContainerColumns.push_back(-startX);
        for (int x = startX + 1; x <= 24; ++x) {
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(-x);
        }
        for (int x = 1; x <= endX; ++x) {
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(x);
        }
        containers.at(8 - 1).at(endX - 1) = temp;
        containerFrames.push_back(containers);
        bufferFrames.push_back(buffer);
        topRowContainerColumns.push_back(0);
        for (int y = 8; y > endY; --y) {
            temp = containers.at(y - 1).at(endX - 1);
            containers.at(y - 1).at(endX - 1) = containers.at(y - 2).at(endX - 1);
            containers.at(y - 2).at(endX - 1) = temp;
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(0);
        }
    }
    else if (isStartTypeShip && !isEndTypeShip) {
        container temp = buffer.at(endY - 1).at(endX - 1);
        buffer.at(endY - 1).at(endX - 1) = containers.at(startY - 1).at(startX - 1);
        containers.at(startY - 1).at(startX - 1) = temp;
        containerFrames.push_back(containers);
        bufferFrames.push_back(buffer);
        topRowContainerColumns.push_back(0);
        for (int y = startY; y < rows; ++y) {
            temp = containers.at(y - 1).at(startX - 1);
            containers.at(y - 1).at(startX - 1) = containers.at(y).at(startX - 1);
            containers.at(y).at(startX - 1) = temp;
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(0);
        }
        temp = containers.at(8 - 1).at(startX - 1);
        containers.at(8 - 1).at(startX - 1) = container();
        containerFrames.push_back(containers);
        bufferFrames.push_back(buffer);
        topRowContainerColumns.push_back(startX);
        for (int x = startX - 1; x >= 1; --x) {
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(x);
        }
        for (int x = 24; x >= endX; --x) {
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(-x);
        }
        buffer.at(4 - 1).at(endX - 1) = temp;
        containerFrames.push_back(containers);
        bufferFrames.push_back(buffer);
        topRowContainerColumns.push_back(0);
        for (int y = 4; y > endY; --y) {
            temp = buffer.at(y - 1).at(endX - 1);
            buffer.at(y - 1).at(endX - 1) = buffer.at(y - 2).at(endX - 1);
            buffer.at(y - 2).at(endX - 1) = temp;
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(0);
        }
    }
    else if (!isStartTypeShip && !isEndTypeShip) {
        container temp = buffer.at(endY - 1).at(endX - 1);
        buffer.at(endY - 1).at(endX - 1) = containers.at(startY - 1).at(startX - 1);
        containers.at(startY - 1).at(startX - 1) = temp;
        containerFrames.push_back(containers);
        bufferFrames.push_back(buffer);
        topRowContainerColumns.push_back(0);
        int highestContainer = top_container_buffer_between(buffer, startX + ((startX < endX) ? 1 : -1), endX);
        bool useTopRow = (highestContainer == 4);
        highestContainer -= useTopRow;
        for (int y = startY; y <= highestContainer; ++y) {
            temp = buffer.at(y - 1).at(startX - 1);
            buffer.at(y - 1).at(startX - 1) = buffer.at(y).at(startX - 1);
            buffer.at(y).at(startX - 1) = temp;
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(0);
        }
        int currContainerRow = max(startY, highestContainer + 1);
        if (useTopRow) {
            temp = buffer.at(4 - 1).at(startX - 1);
            buffer.at(4 - 1).at(startX - 1) = container();
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(-startX);
        }
        if (startX < endX) {
            for (int x = startX; x < endX; ++x) {
                if (!useTopRow) {
                    temp = buffer.at(currContainerRow - 1).at(x - 1);
                    buffer.at(currContainerRow - 1).at(x - 1) = buffer.at(currContainerRow - 1).at(x);
                    buffer.at(currContainerRow - 1).at(x) = temp;
                }
                containerFrames.push_back(containers);
                bufferFrames.push_back(buffer);
                topRowContainerColumns.push_back(0 + -(x + 1) * useTopRow);
            }
        }
        else {
            for (int x = startX; x > endX; --x) {
                if (!useTopRow) {
                    temp = buffer.at(currContainerRow - 1).at(x - 1);
                    buffer.at(currContainerRow - 1).at(x - 1) = buffer.at(currContainerRow - 1).at(x - 2);
                    buffer.at(currContainerRow - 1).at(x - 2) = temp;
                }
                containerFrames.push_back(containers);
                bufferFrames.push_back(buffer);
                topRowContainerColumns.push_back(0 + -(x - 1) * useTopRow);
            }
        }
        if (useTopRow) {
            buffer.at(4 - 1).at(endX - 1) = temp;
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(0);
        }
        for (int y = currContainerRow; y > endY; --y) {
            temp = buffer.at(y - 1).at(endX - 1);
            buffer.at(y - 1).at(endX - 1) = buffer.at(y - 2).at(endX - 1);
            buffer.at(y - 2).at(endX - 1) = temp;
            containerFrames.push_back(containers);
            bufferFrames.push_back(buffer);
            topRowContainerColumns.push_back(0);
        }
    }
    bool interrupt = false;
    while (!interrupt) {
        for (int i = 0; i < containerFrames.size(); ++i) {
            if (GetAsyncKeyState(VK_RETURN) & 0x0001) {
                interrupt = true; break;
            }
            if(GetAsyncKeyState('C') & 0x8000){
                writeComment();
                GetAsyncKeyState(VK_RETURN);
            }
            system("CLS");
            cout << message << "\n\n";
            printShip(containerFrames.at(i), bufferFrames.at(i), topRowContainerColumns.at(i));
            Sleep(200);
        }
        if (interrupt) break;
        for (int i = containerFrames.size() - 2; i >= 1; --i) {
            if (GetAsyncKeyState(VK_RETURN) & 0x0001) {
                interrupt = true; break;
            }
            if(GetAsyncKeyState('C') & 0x8000){
                writeComment();
                GetAsyncKeyState(VK_RETURN);
            }
            system("CLS");
            cout << message << "\n\n";
            printShip(containerFrames.at(i), bufferFrames.at(i), topRowContainerColumns.at(i));
            Sleep(200);
        }
    }
    string successMessage = "Success! Moved " + message.substr(7);
    system("CLS");
    cout << successMessage << "\n\n";
    printShip(n->containers, n->buffer, 0);
    cout << "Step " << currentMoves << "/" << numMoves << " completed." << endl;
    cout << "Remaining time: " << (totalTime - n->totalTime) << " minutes" << endl;
    while (!(GetAsyncKeyState(VK_RETURN) & 0x0001)) {
        if(GetAsyncKeyState('C') & 0x8000){
            writeComment();
        }
        Sleep(100);
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
        for (int x = 1; x <= 24; ++x) {
            if (buffer.at(y - 1).at(x - 1).desc != "UNUSED") {
                return false;
            }
        }
    }
    return true;
}

pair<node*, int> sift(vector<vector<container> >& containers) {
    priority_queue<node*, vector<node*>, CompareNode> nodes;
    node *initial_state = new node(containers);
    vector<vector<container>> goalState = siftGoalState(containers);
    initial_state->set_gn(0);
    initial_state->set_hn(sift_Heuristic(initial_state->containers, initial_state->buffer, goalState));
    nodes.push(initial_state);
    unsigned max_queue_size = 1;
    unsigned nodes_expanded = 0;
    map<string, bool> explored_states;
    while(true) {
        if (nodes.empty()) {
            cout << "ERROR: SIFT operation failed" << endl;
            return pair<node*, int>(nullptr, -1);
        }
        node* curr_state = nodes.top();
        nodes.pop();
        if (isSiftGoalState(curr_state->containers, curr_state->buffer, goalState)) {
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
            return pair<node*, int> (temp, curr_state->totalTime);
        }
        vector<node*> new_nodes = expand(curr_state, nodes, explored_states);
        sift_a_star_search(nodes, new_nodes, goalState);
        ++nodes_expanded;
    }
}

bool isSiftGoalState(const vector<vector<container> >& containers, const vector<vector<container> >& buffer, const vector<vector<container> >& goalState) {
    if (!isBufferEmpty(buffer)) return false;
    for(int i = 1; i <= rows; ++i){
        for(int j = 1; j <= columns; ++j){
            if(containers.at(i - 1).at(j - 1) != goalState.at(i - 1).at(j - 1)){
                return false;
            }
        }
    }
    return true;
}

vector<vector<container>> siftGoalState(vector<vector<container> >& containers){
    vector<container> sortedContainers;
    vector<vector<container>> goalState = containers;
    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){
            if(containers.at(i).at(j).desc != "NAN" && containers.at(i).at(j).desc != "UNUSED" ){
                sortedContainers.push_back(containers.at(i).at(j));
                goalState.at(i).at(j).desc = "UNUSED";
                goalState.at(i).at(j).weight = 0;
            }
        }
    }
    sort(sortedContainers.begin(), sortedContainers.end());
    reverse(sortedContainers.begin(), sortedContainers.end());
    vector<container>::iterator sortedContainerIterator;
    sortedContainerIterator = sortedContainers.begin();
    vector<int> columnsOrder = {6, 7, 5, 8, 4, 9, 3, 10, 2, 11, 1, 12};
    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columnsOrder.size(); ++j){
            if(goalState.at(i).at(columnsOrder.at(j) - 1).desc == "UNUSED"){
                goalState.at(i).at(columnsOrder.at(j) - 1) = *sortedContainerIterator;
                if(++sortedContainerIterator == sortedContainers.end()){
                    return goalState;
                }
            }
        }
    }
    return goalState;
}


void sift_a_star_search(priority_queue<node*, vector<node*>, CompareNode>& nodes, vector<node*>& new_nodes, vector<vector<container>> &goal) {
    for (unsigned i = 0; i < new_nodes.size(); ++i) {
        new_nodes[i]->set_gn(new_nodes[i]->totalTime);
        new_nodes[i]->set_hn(sift_Heuristic(new_nodes[i]->containers, new_nodes[i]->buffer, goal));
        nodes.push(new_nodes[i]);
    } 
}

int sift_Heuristic(vector<vector<container>> &containers, vector<vector<container>> &buffer, vector<vector<container>> &goalState){
    int hn = 0;
    for(int i = 1; i <= rows; ++i){
        for(int j = 1; j <= columns; ++j){
            if(containers.at(i - 1).at(j - 1).desc == "NAN" || containers.at(i - 1).at(j - 1).desc == "UNUSED") continue;
            pair<int, int> location = findContainer(containers.at(i - 1).at(j - 1), goalState);
            hn += (abs((location.first - i)) + abs((location.second - j)));
        }
    }

    for(int i = 1; i <= 4; ++i){
        for(int j = 1; j <= 24; ++j){
            if(buffer.at(i - 1).at(j - 1).desc == "NAN" || buffer.at(i - 1).at(j - 1).desc == "UNUSED") continue;
            pair<int, int> location = findContainer(buffer.at(i - 1).at(j - 1), goalState);
            hn += ((9 - location.first) + (location.second - 1) + 4 + (4 - i) + (24 - j));
        }
    }

    return hn;
}

pair<int, int> findContainer(container &c, vector<vector<container>> &containers){
    for(int i = 1; i <= rows; ++i){
        for(int j = 1; j <= columns; ++j){
            if(c == containers.at(i - 1).at(j - 1)){
                return pair<int, int> (i, j);
            }
        }
    }
    return pair<int,int> (0,0);
}

pair<node*, int> general_search(vector<vector<container> >& containers) {
    priority_queue<node*, vector<node*>, CompareNode> nodes;
    node *initial_state = new node(containers);
    initial_state->set_gn(0);
    initial_state->set_hn(balance_heuristic(initial_state->containers, initial_state->buffer));
    nodes.push(initial_state);
    unsigned max_queue_size = 1;
    unsigned nodes_expanded = 0;
    map<string, bool> added_states;
    added_states[initial_state->to_string()] = true;
    while(true) {
        if (nodes.empty()) {
            cout << "Ship could not be balanced. Beginning SIFT operation." << endl;
            return sift(containers);
            cout << "SIFT operation complete" << endl;
        }
        //if (nodes.size() > max_queue_size) {
        //    max_queue_size = nodes.size();
        //}
        //cout << added_states.size() << '\n'; //debug
        node* curr_state = nodes.top();
        nodes.pop();
        //printShip(curr_state->containers, curr_state->buffer, 0); //DEBUG REMOVE LATER!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
            return pair<node*, int> (temp, curr_state->totalTime);
        }
        vector<node*> new_nodes = expand(curr_state, nodes, added_states);
        a_star_search(nodes, new_nodes);
        ++nodes_expanded;
    }
}

vector<node*> expand(node* curr_state, priority_queue<node*, vector<node*>, CompareNode>& nodes, map<string, bool>& added_states) {
    vector<node*> new_nodes;
    //explored_states[curr_state->to_string()] = true;
    int right_heavier = 6 * (right_mass(curr_state->containers) > left_mass(curr_state->containers));
    for (int i = 1 + right_heavier; i < 7 + right_heavier; ++i)
    {
        node *new_node = new node(curr_state);
        if (top_container(new_node->containers, i) == 0) continue; //no container in column
        if (new_node->containers.at(top_container(new_node->containers, i) - 1).at(i - 1).desc == "UNUSED") continue; //no container in column
        int nodesCreated = 0;
        if(new_node->cranePosX < 0 || new_node->cranePosY < 0){
            new_node->totalTime += ( (5 - abs(new_node->cranePosY)) + (24 - abs(new_node->cranePosX)) + 4);
            new_node->cranePosY = 9;
            new_node->cranePosX = 1;
        }
        //
        int adjustment = 0;
        if (i != new_node->cranePosX) adjustment = (i > new_node->cranePosX) ? -1 : 1;
        int highest_container_crane_pass;
        if (abs(i - new_node->cranePosX) > 1) highest_container_crane_pass = top_container_between(new_node->containers, i + adjustment, new_node->cranePosX - adjustment);
        else highest_container_crane_pass = 0;
        int curr_cell_row = top_container(new_node->containers,i);
        new_node->animationMessage = "Moving SHIP {" + to_string(curr_cell_row) + "," + to_string(i) + "} " + new_node->containers.at(curr_cell_row - 1).at(i - 1).desc + " to ";   
        int timeCraneUp = (highest_container_crane_pass >= new_node->cranePosY) ? highest_container_crane_pass - new_node->cranePosY + 1 : 0;
        int timeCraneHoriz = abs(new_node->cranePosX - i);
        int timeCraneDown = (highest_container_crane_pass >= new_node->cranePosY) ? highest_container_crane_pass - curr_cell_row : abs(new_node->cranePosY - curr_cell_row);
        new_node->totalTime += timeCraneUp + timeCraneHoriz + timeCraneDown;
        //new_node->animationMessage += to_string(timeCraneUp) + "_"; //debug
        //new_node->animationMessage += to_string(timeCraneHoriz) + "_"; //debug
        //new_node->animationMessage += to_string(timeCraneDown) + "_"; //debug
        //
        for(int j = 1; j <= columns; ++j){
            node *new_node_ship = new node(new_node);
            if (top_container(new_node_ship->containers, j) == rows) continue; //cannot put container in full column
            if(j == i) continue;  //moving in the same row, basically moving no where
            adjustment = (j > i) ? -1 : 1;
            int highest_container;
            if (abs(j - i) > 1) highest_container = top_container_between(new_node_ship->containers, j + adjustment, i - adjustment);
            else highest_container = 0;
            int timeContainerUp = (highest_container >= top_container(new_node_ship->containers, i)) ? highest_container - top_container(new_node_ship->containers, i) + 1 : 0;
            int timeContainerHoriz = abs(j - i);
            int timeContainerDown = (highest_container >= top_container(new_node_ship->containers, i)) ? highest_container - top_container(new_node_ship->containers, j) : abs(top_container(new_node_ship->containers, i) - top_container(new_node_ship->containers, j) - 1);
            new_node_ship->totalTime += timeContainerUp + timeContainerHoriz + timeContainerDown;
            //new_node_ship->animationMessage += to_string(timeContainerUp) + "_"; //debug
            //new_node_ship->animationMessage += to_string(timeContainerHoriz) + "_"; //debug
            //new_node_ship->animationMessage += to_string(timeContainerDown) + "_"; //debug
            int new_cell_row = top_container(new_node_ship->containers, j) + 1;
            container temp = new_node_ship->containers.at(curr_cell_row - 1).at(i - 1);
            new_node_ship->containers.at(curr_cell_row - 1).at(i - 1) = new_node_ship->containers.at(new_cell_row - 1).at(j - 1);
            new_node_ship->containers.at(new_cell_row - 1).at(j - 1) = temp;
            new_node_ship->cranePosY = new_cell_row;
	        new_node_ship->cranePosX = j;
            new_node_ship->animationMessage += "SHIP {" + to_string(new_cell_row) + "," + to_string(j) + "}";
            if (added_states[new_node_ship->to_string()] == false) {
                added_states[new_node_ship->to_string()] = true;
                new_nodes.push_back(new_node_ship);
                nodesCreated++;
            }
        }
        if(nodesCreated == 0) {
            pair<int, int> bp = findNearestBufferColumn(new_node->buffer);
            int closestBufferColumn = bp.first;
            int closestBufferRow = top_container_buffer(new_node->buffer, closestBufferColumn) + 1;
            new_node->totalTime += bp.second;
            //printShip(new_node->containers, new_node->buffer, 0);
            container temp = new_node->containers.at(curr_cell_row - 1).at(i - 1);
            new_node->containers.at(curr_cell_row - 1).at(i - 1) = new_node->buffer.at(closestBufferRow - 1).at(closestBufferColumn - 1);
            new_node->buffer.at(closestBufferRow - 1).at(closestBufferColumn - 1) = temp;
            new_node->cranePosY = -1 * closestBufferRow;
	        new_node->cranePosX = -1 * closestBufferColumn;
            new_node->animationMessage += "BUFFER {" + to_string(closestBufferRow) + "," + to_string(closestBufferColumn) + "}";
        }
        if (added_states[new_node->to_string()] == false) {
            added_states[new_node->to_string()] = true;
            new_nodes.push_back(new_node);
            nodesCreated++;
        }
    }
    if(!isBufferEmpty(curr_state->buffer)){
        for (int i = 1; i <= 24; ++i){
            node *new_node = new node(curr_state);
            if (top_container_buffer(new_node->buffer, i) == 0) continue; //no container in column
            if(new_node->cranePosX > 0 || new_node->cranePosY > 0){
                new_node->totalTime += ( (9 - new_node->cranePosY) + (new_node->cranePosX - 1) + 4);
                new_node->cranePosY = -5;
                new_node->cranePosX = -24;
            }
            int adjustment = 0;
            if (i != abs(new_node->cranePosX)) adjustment = (i > abs(new_node->cranePosX)) ? -1 : 1;
            int highest_container_crane_pass;
            if (abs(i - abs(new_node->cranePosX)) > 1) highest_container_crane_pass = top_container_buffer_between(new_node->buffer, i + adjustment, abs(new_node->cranePosX) - adjustment);
            else highest_container_crane_pass = 0;
            int curr_cell_row = top_container_buffer(new_node->buffer, i);
            new_node->animationMessage = "Moving BUFFER {" + to_string(curr_cell_row) + "," + to_string(i) + "} " + new_node->buffer.at(curr_cell_row - 1).at(i - 1).desc + " to ";

            int timeCraneUp = (highest_container_crane_pass >= abs(new_node->cranePosY)) ? highest_container_crane_pass - abs(new_node->cranePosY) + 1 : 0;
            int timeCraneHoriz = abs(abs(new_node->cranePosX) - i);
            int timeCraneDown = (highest_container_crane_pass >= abs(new_node->cranePosY)) ? highest_container_crane_pass - curr_cell_row : abs(abs(new_node->cranePosY) - curr_cell_row);
            new_node->totalTime += timeCraneUp + timeCraneHoriz + timeCraneDown;

            for(int j = 1; j <= columns; ++j){
                node *new_node_ship = new node(new_node);
                if (top_container(new_node_ship->containers, j) == rows) continue; //cannot put container in full column
                int cell_row = top_container(new_node_ship->containers, j) + 1;
                int timeContainerBuffer = (5 - curr_cell_row) + (24 - i);
                int timeContainerBtoS = 4;
                int timeContainerShip = (9 - cell_row) + (j - 1);
                new_node_ship->totalTime += timeContainerBuffer + timeContainerBtoS + timeContainerShip;
                //new_node_ship->totalTime += (abs(abs(new_node_ship->cranePosY) - curr_cell_row) + abs((abs(new_node_ship->cranePosX)) - i)); //Don't know why this was here
                container temp = new_node_ship->buffer.at(curr_cell_row - 1).at(i - 1);
                new_node_ship->buffer.at(curr_cell_row - 1).at(i - 1) = new_node_ship->containers.at(cell_row - 1).at(j - 1);
                new_node_ship->containers.at(cell_row - 1).at(j - 1) = temp;
                new_node_ship->cranePosY = cell_row;
	            new_node_ship->cranePosX = j;
                new_node_ship->animationMessage += "SHIP {" + to_string(cell_row) + "," + to_string(j) + "}";
                if (added_states[new_node_ship->to_string()] == false) {
                    added_states[new_node_ship->to_string()] = true;
                    new_nodes.push_back(new_node_ship);
                }
            }
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


int top_container_buffer(vector<vector<container> >& buffer, int column) {
    for (int y = 4; y >= 1; --y) {
        string desc = buffer.at(y - 1).at(column - 1).desc;
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

int top_container_buffer_between(vector<vector<container> >& buffer, int column1, int column2) {
    int highest_container = 0;
    int lower = min(column1, column2);
    int upper = max(column1, column2);
    for (int i = lower; i <= upper; ++i) {
        if (top_container_buffer(buffer, i) > highest_container) {
            highest_container = top_container_buffer(buffer, i);
        }
    }
    return highest_container;
}

void a_star_search(priority_queue<node*, vector<node*>, CompareNode>& nodes, vector<node*>& new_nodes) {
    for (unsigned i = 0; i < new_nodes.size(); ++i) {
        new_nodes[i]->set_gn(new_nodes[i]->totalTime);
        new_nodes[i]->set_hn(balance_heuristic(new_nodes[i]->containers, new_nodes[i]->buffer));
        nodes.push(new_nodes[i]);
    }
}

int balance_heuristic(vector<vector<container> >& containers, vector<vector<container> >& buffer) {
    int heavier_side_weight = left_mass(containers);
    int lighter_side_weight = right_mass(containers);
    if (lighter_side_weight > heavier_side_weight) {
        swap(lighter_side_weight, heavier_side_weight);
    }
    int hn_weight = heavier_side_weight * 0.9 - lighter_side_weight;
    int buffer_weight = 0;
    for (int i = 1; i <= 4; ++i) {
        for (int j = 1; j <= 24; ++j) {
            buffer_weight += buffer.at(i - 1).at(j - 1).weight;
        }
    }
    return hn_weight + buffer_weight * 0.5;
    //( (heavier_side_weight - lighter_side_weight) / (heavier_side_weight + lighter_side_weight)) * 10 + totalTime
}


