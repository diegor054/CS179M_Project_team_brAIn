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









    string shipName = "ShipCase5.txt";
    ifstream fin(shipName);
    if (!fin.is_open()) {
        cout << "Error opening " << shipName << "!" << endl;
        exit(EXIT_FAILURE);
    }
    string coordinates, weight, description;

    while (fin >> coordinates) {
        fin >> weight >> description;

    }
    fin.close();





    return 0;
}