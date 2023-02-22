#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <windows.h>

using namespace std;

struct container {
    int x;
    int y;
    int weight;
    string desc;
    container(int y, int x, int w, string d) : y(y), x(x), weight(w), desc(d) { };
};

void readManifest(const string&, vector<container>&);
void debugManifest(const vector<container>&);
void debugASCII();
void printShip(const vector<container>&);
void moveContainer(const vector<container>&, int, int, int);
void log_File(ofstream& logFile, string message);
void logIn(ofstream& logFile);
void menu(ofstream& logFile);
string userLoggedIn;  //global variable 

HANDLE console_color = GetStdHandle(STD_OUTPUT_HANDLE);

const int rows = 8, columns = 12;

ofstream logFile;

int main() {
    logFile.open ("log.txt");
    logIn(logFile);
    menu(logFile);
 
    SetConsoleTextAttribute(console_color, 0x01);

    string file = "manifests\\ShipCase2.txt";
    vector<container> containers;
    readManifest(file, containers);
    //debugManifest(containers);
    //debugASCII();
    
    printShip(containers);
    system("PAUSE");
    logFile.close();
    return 0;
}

void readManifest(const string &manifest, vector<container> &containers) {
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
        containers.emplace_back(y, x, w, description);
        if (description != "NAN" && description != "UNUSED") ++numContainers;
    }
    string message = "Manifest " + manifest + " is opened, there are " + to_string(numContainers) + " containers on the ship.";
    log_File(logFile, message);
    fin.close();
}

void debugManifest(const vector<container> &containers) {
    for (int i = 0; i < containers.size(); ++i) {
        cout << "[" << containers.at(i).x << "," << containers.at(i).y << "] "
             << containers.at(i).weight << "kgs: {" << containers.at(i).desc << "}" << endl; 
    }
}

void debugASCII() {
    for (unsigned char c = 32; c != 255; ++c) {
        cout << "[" << (int)c << "]" << c << endl;
    }
}

void printShip(const vector<container> &containers) {
    //[176]░ [177]▒ [178]▓ [219]█ [254]■
    string dsc;
    for (int y = rows; y >= 1; --y) {
        cout << (char)219;
        for (unsigned x = 1; x <= columns; ++x) {
            dsc = containers.at((y - 1) * columns + (x - 1)).desc;
            if (dsc == "NAN") cout << (char)219;
            else if (dsc == "UNUSED") cout << (char)32;
            else {
                SetConsoleTextAttribute(console_color, 0x04);
                cout << (char)178;
                SetConsoleTextAttribute(console_color, 0x01);
            }
        }
        cout << (char)219 << endl;
    }
    for (int i = 0; i < columns + 2; ++i) cout << (char)219;
    return;
}

void moveContainer(const vector<container> &containers, int x, int y, int dir) {
    //0 = UP, 1 = DOWN, 2 = LEFT, 3 = RIGHT
    int index = (y - 1) * columns + (x - 1);
    switch(dir) {
        case 0:
            if (y == 1) cout << "Error Up" << endl;
            //containers.at(index - columns) = containers.at(index);
            break;
        case 1:
            if (y == rows) cout << "Error Down" << endl;
            //containers.at(index + columns) = containers.at(index);
            break;
        case 2:
            if (x == 1) cout << "Error Left" << endl;
            //containers.at(index - 1) = containers.at(index);
            break;
        case 3:
            if (x == columns) cout << "Error Right" << endl;
            //containers.at(index + 1) = containers.at(index);
            break;
        default:
            cout << "Error Unknown" << endl;
            break;
    }
}


void log_File(ofstream& logFile, string message){
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

void logIn(ofstream& logFile){
    cout << "Welcome! Enter your name to log in: " << flush;
    string name;
    while(name == "") getline(cin, name);

    /*
    if(userLoggedIn != ""){
    string logOutMessage = userLoggedIn + " logged out.";
    log_File(logFile , logOutMessage);
    }
    */

    string logInMessage = name +  " logged in.";
    log_File(logFile, logInMessage);

    userLoggedIn = name;
}

void menu(ofstream& logFile) {
    int option = 0;

    while(option != 1 && option != 2 && option != 3) {
        system("cls");
        if (option) cout << "Invalid option. Please try again." << '\n';
        cout << "1. Switch users" << endl;
        cout << "2. View balance weight" << endl;
        cout << "3. Load/Unload" << endl;
        cout << "Enter option: ";
        cin >> option;
    }

    switch (option) {
        case 1: {
            logIn(logFile);
            break;
        }
        case 2:
            //balanceShip()
            break;
        case 3:
            //unload_load()
            break;
        default:
            break;
    }
}