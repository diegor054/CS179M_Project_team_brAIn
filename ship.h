#ifndef SHIP_H
#define SHIP_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace std;

struct container {
    int weight;
    string contents;
    container() : weight(0), desc("UNUSED") { };
    container(int w, string c) 
    {
        if (c == "NAN") { weight = 0 }
      else { weight = w }
      contents = c;
    };
};

#endif