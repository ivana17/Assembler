#ifndef _SECTION_H_
#define _SECTION_H_

#include <iostream>
#include <map>
#include <string>

using namespace std;

class Section{
public:
    Section(string _name, int _size);
    Section(){cout<<"idioti"<<endl;}
    string name;
    int size;
    map<int, string> content;
    
    void writeZeroBytes(int offs, int len);
    void writeByte(int offs, string _byte);
    void writeBytes(int offs, string _bytes);

    ~Section();

private:

};

#endif