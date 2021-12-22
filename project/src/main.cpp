#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

#include "assembler.h"
using namespace std;

int main(int argc, char* argv[]){

    // asembler -o ulaz1.o ulaz1.s  // asembler ulaz1.s -o ulaz1.o 
    string inFileName, outFileName;
    if(argc == 4 && (strcmp(argv[1], "-o") == 0 || strcmp(argv[2], "-o") == 0)){
        if(strcmp(argv[1], "-o") == 0){
            outFileName = argv[2];
            inFileName = argv[3];
        } else{
            inFileName = argv[1];
            outFileName = argv[3];
        }
    } else {
        cout << "Invalid arguments." << endl;
        return 1;
    }

    ifstream inFile(inFileName);
    ofstream outFile(outFileName);
    if(!inFile.is_open() || !outFile.is_open()){
        cout << "Error opening file" << endl;
        return 2;
    }

    Assembler* assembler = new Assembler(inFile, outFile);
    assembler->compile();

    inFile.close();
    outFile.close();
    delete assembler;
	cout << "Compiled! :)" << endl;
	return 0;
}