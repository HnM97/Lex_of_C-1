#include "lex.h"

lex::lex() {
    integer.open("integer.txt");
    float_number.open("float.txt");
    identifier.open("identifier.txt");

    keywords = new trie();
    ifstream key("keyword.txt");
    if(!key.is_open()) {
        printf("Error: Opening file -- %s\n", "keyword.txt");
        exit(-1);
    }
    string in;
    while(getline(key, in))
        keywords->insert(in.c_str());
    key.close();

    ifstream sym("symbol.txt");
    if(!sym.is_open()) {
        printf("Error: Opening file -- %s\n", "symbol.txt");
        exit(-1);
    }
    while(getline(sym, in)) {
        istringstream ss(in);
        string s, tt;
        getline(ss, s, ' '); getline(ss, tt, ' ');
        symbols[s[0]] = tt;
    }
    sym.close();

    ifstream op("operator.txt");
    if(!op.is_open()) {
        printf("Error: Opening file -- %s\n", "operator.txt");
        exit(-1);
    }
    while(getline(op, in)) {
        istringstream ss(in);
        string s, tt;
        getline(ss, s, ' '); getline(ss, tt, ' ');
        operators[s[0]] = tt;
    }
    op.close();
}

void lex::init() {
    lexeme.clear();
    line = -1;
}

void lex::analyze(string fn) {
    init();

    ifstream source(fn);
    if(!source.is_open()) {
        cout << "Error: Opening file -- " << fn << endl;
        exit(-1);
    }

    string in, tok;
    int el = 0, ec = 0;
    while(getline(source, in)) {
        line += 1;
        col = -1;
        bool f = false;
        int n = in.length();
        for(int i = 0;i < n;++i) {
            char c = in[i];
            col += 1;
            // is c symbol?
            if(isSymbol(c)) {                
                getToken(tok, el, ec);
                el = line; ec = col;
                f = false;
                lexeme.push_back((token){symbols[c], el, ec, {0}});
            }
            else if(isWS(c)) {
                getToken(tok, el, ec);
                el = line; ec = col;
                f = false;
                lexeme.push_back((token){"WS", el, ec, {0}});
            }
            else if(isOp(c) || c == '!') {
                int tl = el, tc = ec;
                el = line; ec = col;
                f = false;
                // composite operator
                bool fl = false;
                if(i + 1 < n) {
                    char lookahead = in[i + 1];
                    struct comp {
                        char a, b;
                        string tt;
                    } compOp[] = { {'<', '<', "LSHIFT"}, {'<', '=', "LEQ"}, {'>', '>', "RSHIFT"}, 
                                   {'>', '=', "GEQ"}, {'=', '=', "EQ"}, {'!', '=', "NEQ"} };
                    for(int j = 0;j < 6;++j) {
                        if(compOp[j].a == c && compOp[j].b == lookahead) {
                            i++;
                            lexeme.push_back((token){compOp[j].tt, el, ec, {0}});
                            fl = true;
                        }
                    }
                }
                // single operator
                if(!fl) {
                    if(c == '!') tok += c;
                    else {
                        getToken(tok, el, ec);
                        lexeme.push_back((token){operators[c], el, ec, {0}});
                    }
                }
            }
            else if(c == '\"') {
                getToken(tok, el, ec);
                el = line; ec = col;
                f = false;
                string lit;
                lit += c;
                if(i == n - 1) errorOccur("Invalid string format: ", el, ec);
                i++;
                while(i < n && in[i] != '\"') lit += in[i++];
                if(i >= n || in[i] != '\"') errorOccur("Invalid string format: ", el, ec);
                lit += in[i];
                char *s = new char[MAX_ST_LEN]();
                strncpy(s, lit.c_str(), lit.length());
                lexeme.push_back((token){"LITERAL", el, ec, {.s = s}});
            }
            else {
                tok += c;
                if(!f) {
                    el = line;
                    ec = col;
                    f = true;
                }
            } // if
        } // for
        if(tok.size()) getToken(tok, el, ec);
    } // while

    string fout;
    istringstream ss(fn);
    getline(ss, fout, '.');
    fout += ".out";
    FILE *out = fopen(fout.c_str(), "w");
    if(out == NULL) {
        printf("Error: Opening file -- output.txt\n");
        exit(-1);
    }
    for(auto t : lexeme) {
        fprintf(out, "%20s %5d %5d", t.tokentype.c_str(), t.el, t.ec);
        if(t.tokentype == "ID" || t.tokentype == "LITERAL")
            fprintf(out, ": %-20s\n", t.s);
        else if(t.tokentype == "INTEGER")
            fprintf(out, ": %-10d\n", t.n);
        else if(t.tokentype == "FLOAT")
            fprintf(out, ": %-10f\n", t.f);
        else fprintf(out, "\n");
    }
    fclose(out);

    source.close();
}

void lex::getToken(string &tok, int el, int ec) {
    if(tok.empty()) return;

    if(keywords->find(tok.c_str())) lexeme.push_back({tok, 0, 0, {0}});
    else if(identifier.accept(tok)) {
        char *id = new char[MAX_ID_LEN]();
        strncpy(id, tok.c_str(), tok.length());
        lexeme.push_back((token){"ID", 0, 0, {.s = id}});
    }
    else if(integer.accept(tok)) {
        int n = stoi(tok);
        lexeme.push_back((token){"INTEGER", 0, 0, {.n = n}});
    }
    else if(float_number.accept(tok)) {
        float f = stof(tok);
        lexeme.push_back((token){"FLOAT", 0, 0, {.f = f}});
    }
    else errorOccur("Invalid format: ", el, ec);
    tok.clear();
}

bool lex::isSymbol(char c) {
    return symbols.count(c) > 0;
}

bool lex::isWS(char c) {
    return (c == ' ' || c == '\n' || c == '\t');
}

bool lex::isOp(char c) {
    return operators.count(c) > 0;
}

void lex::errorOccur(string msg, int l, int c) {
    printf("%s %5d %5d\n", msg.c_str(), l, c);
    exit(-1);
}