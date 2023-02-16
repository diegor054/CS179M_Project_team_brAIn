#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;

struct container {
    int x;
    int y;
    int weight;
    string desc;
    container(int x, int y, int w, string d) : x(x), y(y), weight(w), desc(d) { };
};

void readManifest(const string&, vector<container>&);
void debugManifest(const vector<container>&);
void debugASCII();
void printShip(const vector<container>&);

HANDLE console_color = GetStdHandle(STD_OUTPUT_HANDLE);

int main() {

    SetConsoleTextAttribute(console_color, 0x01);

    string file = "ShipCase2.txt";
    vector<container> containers;
    readManifest(file, containers);
    debugManifest(containers);
    debugASCII();
    
    printShip(containers);

    system("PAUSE");

    return 0;
}

void readManifest(const string &manifest, vector<container> &containers) {
    ifstream fin(manifest);
    if (!fin.is_open()) {
        cout << "Error opening " << manifest << "!" << endl;
        exit(EXIT_FAILURE);
    }
    string coordinates, weight, description;
    while (fin >> coordinates) {
        fin >> weight >> description;
        int x = (coordinates.at(1) - 0x30) * 10 + (coordinates.at(2) - 0x30);
        int y = (coordinates.at(4) - 0x30) * 10 + (coordinates.at(5) - 0x30);
        int w = stoi(weight.substr(1, 5));
        containers.emplace_back(x, y, w, description);
    }
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
    int rows = 8, columns = 12;
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