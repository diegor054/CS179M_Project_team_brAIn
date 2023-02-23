#include <iostream>
#include <vector>

using namespace std;

struct container {
    int y;
    int x;
    int weight;
    string desc;
    container(int y, int x, int w, string d) : y(y), x(x), weight(w), desc(d) { };
    container operator=(container c) {y = c.y; x = c.x; weight = c.weight; desc = c.desc; return c;}
};

struct node {
    vector<container> containers;
    int fn, gn, hn;
    node(vector<container> c, int g) : containers(c), gn(g) {init();}
    node(node &n) : containers(n.containers), gn(n.gn + 1) {init();}
    void init() {hn = get_hn(containers); fn = gn + hn;}
    container getContainer(int y, int x) {return containers.at((y - 1) * columns + (x - 1));}
};

const int rows = 8, columns = 12;

int left_mass(const vector<container>&);
int right_mass(const vector<container>&);

int main() {
    container a(1, 1, 98, "a");
    container b(1, 2, 1, "b");
    container c(1, rows, 91, "c");
    vector<container> containers;
    containers.push_back(a);
    containers.push_back(b);
    containers.push_back(c);
    cout << "The left side has a combined mass of " << left_mass(containers) << " kgs." << endl;
    cout << "The right side has a combined mass of " << right_mass(containers) << " kgs." << endl;
    return 0;
}

int left_mass(const vector<container>& containers) {
    int mass = 0;
    for (int i = 0; i < containers.size(); ++i) {
        if (containers.at(i).x <= (rows / 2)) {
            mass += containers.at(i).weight;
        }
    }
    return mass;
}

int right_mass(const vector<container>& containers) {
    int mass = 0;
    for (int i = 0; i < containers.size(); ++i) {
        if (containers.at(i).x > (rows / 2)) {
            mass += containers.at(i).weight;
        }
    }
    return mass;
}

double deficit(const vector<container>& containers) {
    return abs(left_mass(containers) - right_mass(containers)) / 2.0;
}

int get_hn(vector<container>& containers) {
    int hn;


    return hn;
}