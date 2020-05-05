#ifndef LEX_H_INCLUDED
#define LEX_H_INCLUDED

#include "dfa.h"

#define MAX_N 128
#define MAX_ID_LEN 1000
#define MAX_ST_LEN 1000

struct trie {
    trie() { 
        for(int i = 0;i < MAX_N;++i) next[i] = 0;
        isTerminal = false;
    }
    ~trie() {
        for(int i = 0;i < MAX_N;++i)
            if(next[i]) delete(next[i]);
    }

    bool find(const char *key) {
        if(*key == '\0') return isTerminal;
        else {
            int cur = *key;
            if(next[cur]) return next[cur]->find(key + 1);
            else return false;
        }
    }

    void insert(const char *key) {
        if(*key == '\0') isTerminal = true;
        else {
            int cur = *key;
            if(!next[cur]) next[cur] = new trie;
            next[cur]->insert(key + 1);
        }
    }

    trie *next[MAX_N];
    bool isTerminal;
};

struct token {
    string tokentype;
    int el, ec;
    union {
        char *s;
        int n;
        float f;
    };
};

typedef pair<char, string> pcs;

class lex {
public:
    lex();
    ~lex() {
        delete(keywords);
    }
    void analyze(string);
private:
    trie *keywords;
    dfa integer, float_number, identifier;
    map<char, string> symbols, operators;
    vector<token> lexeme;

    int line, col;

    bool isSymbol(char);
    bool isWS(char);
    bool isOp(char);
    void errorOccur(string, int, int);
    void getToken(string &, int, int);
    void init();
};

#endif