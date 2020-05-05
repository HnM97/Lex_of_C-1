#include "dfa.h"

void dfa::open(string fn) {
    // opening file
    ifstream source(fn);
    if(!source.is_open()) {
        cout << "Error: Opening file -- " << fn << endl;
        exit(-1);
    }

    curState = "0";

    // get dfa from source
    // finalStates
    string in;
    getline(source, in);
    vector<string> v;
    getData(in, v);
    for(auto s : v) finalStates.insert(s);
    // alphabet
    v.clear();
    getline(source, in);
    getData(in, v);
    for(auto s : v) alphabet.insert(s);
    // transitions
    while(getline(source, in)) {
        v.clear();
        getData(in, v);
        string f = v[0], i = v[1], t = v[2];

        if(!alphabet.count(i)) {
            cout << "Invalid alphabet from dfa transition -- " << fn << endl;
            exit(-1);
        }
        transitions[pss(f, i)] = t;
    }
    source.close();
    isOpen = true;
}

bool dfa::accept(string s) {
    if(!isOpen) return false;
    if(s.empty()) return true;

    curState = "0";
    // running dfa
    for(char c : s) {
        if(!alphabet.count(string(1, c))) return false;

        auto delta = pss(curState, string(1, c));
        if(transitions.count(delta))
            curState = transitions[delta];
        else return false;
    }
    // is the curState final?
    return finalStates.count(curState) > 0;
}

void getData(string &in, vector<string> &ret) {
    istringstream ss(in);
    string s;
    while(!ss.eof()) {
        getline(ss, s, ' ');
        ret.push_back(s);
    }
}