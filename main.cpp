#include <iostream>
#include <fstream>
#include <string>
#include <vector>
//#include <pair>

using namespace std;

struct container {
    int x;
    int y;
    int weight;
    string desc;
    container(int x, int y, int w, string d) : x(x), y(y), weight(w), desc(d) { };
};

int main() {











    //MANIFEST CODE
    string shipName = "ShipCase5.txt";
    ifstream fin(shipName);
    if (!fin.is_open()) {
        cout << "Error opening " << shipName << "!" << endl;
        exit(EXIT_FAILURE);
    }
    string coordinates, weight, description;
    vector<container> containers;
    while (fin >> coordinates) {
        fin >> weight >> description;
        int x = (coordinates.at(1) - 0x30) * 10 + (coordinates.at(2) - 0x30);
        int y = (coordinates.at(4) - 0x30) * 10 + (coordinates.at(5) - 0x30);
        int w = stoi(weight.substr(1, 5));
        containers.emplace_back(x, y, w, description);
    }
    fin.close();

    //debug
    for (int i = 0; i < containers.size(); ++i) {
        cout << "[" << containers.at(i).x << "," << containers.at(i).y << "] "
             << containers.at(i).weight << "kgs: {" << containers.at(i).desc << "}" << endl; 
    }

    //debug ascii
    for (unsigned char c = 32; c != 255; ++c) {
        cout << "[" << (int)c << "]" << c << endl;
    }

    //[176]░ [177]▒ [178]▓ [219]█ [254]■
    return 0;
}