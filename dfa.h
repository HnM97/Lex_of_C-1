#ifndef DFA_H_INCLUDED
#define DFA_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <utility>
#include <vector>

using namespace std;

typedef pair<string, string> pss;

class dfa {
public:
    dfa() { isOpen = false; }
    ~dfa() {}
    void open(string);
    bool accept(string);
private:
    set<string> alphabet;
    set<string> finalStates;
    map<pss, string> transitions;
    string curState;
    bool isOpen;
};

void getData(string &, vector<string> &);

#endif