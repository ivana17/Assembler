#include "symbol.h"

int Symbol::IDs = -1;

Symbol::Symbol(string lab, SectionType sec, int offs, ScopeType _scope, TokenType tok, int _size, bool def){
     
    serialNum = ++IDs; 
    label = lab;
    section = sec;
    offset = offs;
    scope = _scope;
    symType = tok;
    int size = _size;
    defined = def;
    
}
