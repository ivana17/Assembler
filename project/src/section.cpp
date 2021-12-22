#include "section.h"

Section::Section(string _name, int _size): 
            name(_name), size(_size){ }


void Section::writeZeroBytes(int offs, int len){
    int i = 0;
    while(i < len){
        pair<int, string> val(offs + i++, "00");
        content.insert(val);
    }
}

void Section::writeByte(int offs, string _byte){
    pair<int, string> val(offs, _byte);
    content.insert(val);   
}

void Section::writeBytes(int offs, string _bytes){
    pair<int, string> val(offs, _bytes);
    content.insert(val);    
}

Section::~Section(){ }