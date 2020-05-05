#include <iostream>
#include "dfa.h"
#include "lex.h"

using namespace std;

int main(int argc, char *argv[]) {
    lex clex;
    clex.analyze(argv[1]);

    return 0;
}