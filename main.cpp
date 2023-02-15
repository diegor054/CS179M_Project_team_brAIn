#include <iostream>
#include <fstream>
#include <string>
//#include <pair>

using namespace std;

int main() {









    string shipName = "ShipCase5.txt";
    ifstream fin(shipName);
    if (!fin.is_open()) {
        cout << "Error opening " << shipName << "!" << endl;
        exit(EXIT_FAILURE);
    }
    string coordinates, weight, description;
    while (fin >> coordinates) {
        
    }
    fin.close();





    return 0;
}