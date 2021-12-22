#ifndef _RELOC_H_
#define _RELOC_H_

#include <iostream>
#include <string>

using namespace std;

enum RelocType { ABS, PCREL };

class Reloc{
public:
    Reloc(string _name, string sec, int offs, RelocType _type, int add);
	string name;
	string section;
    int offset;
    RelocType type;
	int addend;

    friend ostream& operator<<(ostream& os, const Reloc& rel);
    ~Reloc();
};

#endif