#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

enum ScopeType { LOCAL, GLOBAL, EXTERN };
enum SectionType { START, TEXT, DATA, BSS, RODATA, UND };
enum TokenType { LABEL, SECTION, SYMBOL, EXT_GLB, INSTRUCTION, INCORRECT, DIRECTIVE, OP_DEC, EQU, END };

struct forw_ref {
	int patch;
	string name;
	int equ_sign;
	forw_ref(int p , string s = "", int sign = 1): patch(p), name(s), equ_sign(sign) { }
};


class Symbol{
public:
    Symbol(string lab, SectionType sec, int offs, ScopeType _scope, TokenType tok, int _size, bool def);
    static int IDs; 
    int serialNum; 

    string label;
    SectionType section;
    int offset; //value (case equ)
    ScopeType scope;
    bool defined;
    vector<forw_ref> flink;
    TokenType symType;
	int size; //numOfUndefinedSymbols (case equ)

};

#endif