#include <iostream>
#include <string>

using namespace std;

void menu(string& username, double& balance_weight) {
    int option;

    cout << "1. Switch users" << endl;
    cout << "2. View balance weight" << endl;
    cout << "3. Load/Unload" << endl;

    cout << "Enter option: ";
    cin >> option;

    switch (option) {
        case 1: {
            string new_username;
            cout << "Enter new username: ";
            cin >> new_username;
            
            username = new_username;
            break;
        }
        case 2:
            cout << "Balance weight: " << balance_weight << " lbs" << endl;
            break;
        case 3:
            cout << "You can now begin loading and unloading." << endl;
            break;
        default:
            cout << "Invalid option." << endl;
            break;
    }
}

int main() {
    string username;
    double balance_weight = 0.0;

    cout << "Enter username: ";
    cin >> username;

    menu(username, balance_weight);

    return 0;
}
