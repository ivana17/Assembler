#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include <vector>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <regex>

#include "section.h"
#include "symbol.h"
#include "reloc.h"

using namespace std;

enum Instruction { HALT, IRET, RET, INT, CALL, JMP, JEQ, JNE, JGT, PUSH, POP, XCHG, 
                    MOV, ADD, SUB, MUL, DIV, CMP, NOT, AND, OR, XOR, TEST, SHL, SHR };
enum OperandType { op_dec, op_sym_val, op_reg_ind, op_sym_mem, op_mem, op_reg, op_reg_ind_val, op_reg_ind_sym, op_pcrel,
                    jmp_op_dec, jmp_op_sym_val, jmp_op_reg_ind, jmp_op_sym_mem, jmp_op_mem, jmp_op_reg, jmp_op_reg_ind_val, jmp_op_reg_ind_sym, jmp_op_pcrel };

class Assembler{
public:

    Assembler(ifstream& in, ofstream& out);
    ~Assembler();

    void compile();

private:

    int locationCnt;
    ofstream& outputFile;
    vector<queue<string>> asmInput;

    static map<SectionType, string> sectionCode;
    static map<Instruction, int> instrNumOper;
    static map<string, Instruction> instrName;
    static map<Instruction, string> instrOpCode;
    static map<OperandType, regex> opTypeRgx;

	vector<string> symbols;
    map<string, Symbol> symbolTable;
    unordered_map<string, Section> sections;
    vector<Reloc> relocations;

    SectionType currSection;
    TokenType currToken;
    bool jmpFlag;

    void parseInput(ifstream& in);

    void addSymbol(string, SectionType, int, ScopeType, TokenType, int, bool);
	void updateSymbol(string, SectionType, int, TokenType, bool);
    TokenType tokenParser(string);
    void directiveHandler(string, queue<string>&, string&);
    void instructionHandler(string, queue<string>&);
    int addressingMode(string);
    int operandParser(string&, int&, int&);

    int setAbsReloc(string, int, int);
    int setPCrelReloc(string, int, int);

    string decToHex(int, int);
};

#endif