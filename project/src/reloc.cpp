#include "reloc.h" 

Reloc::Reloc(string _name, string sec, int offs, RelocType t, int add):
                name(_name), section(sec), offset(offs), type(t), addend(add) { }

ostream& operator<<(ostream& os, const Reloc& rel){
    if(rel.type == ABS)
        os << hex << rel.offset << " R_x86_64_32" << "  " << rel.section;
    else
        os << hex << rel.offset << " R_x86_64_PC32" << "      " << rel.name << " " << rel.addend;
	return os << endl;
}

Reloc::~Reloc(){ }