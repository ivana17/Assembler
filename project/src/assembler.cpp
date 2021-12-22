#include <iomanip>
#include "assembler.h"

map<Instruction, int> Assembler::instrNumOper = {
    { Instruction::HALT, 0 },
    { Instruction::IRET, 0 },
    { Instruction::RET, 0 },
    { Instruction::INT, 1 },
    { Instruction::CALL, 1 },
    { Instruction::JMP, 1 },
    { Instruction::JEQ, 1 },
    { Instruction::JNE, 1 },
    { Instruction::JGT, 1 },
    { Instruction::PUSH, 1 },
    { Instruction::POP, 1 },
    { Instruction::XCHG, 2 },
    { Instruction::MOV, 2 },
    { Instruction::ADD, 2 },
    { Instruction::SUB, 2 },
    { Instruction::MUL, 2 },
    { Instruction::DIV, 2 },
    { Instruction::CMP, 2 },
    { Instruction::NOT, 2 },
    { Instruction::AND, 2 },
    { Instruction::OR, 2 },
    { Instruction::XOR, 2 },
    { Instruction::TEST, 2 },
    { Instruction::SHL, 2 },
    { Instruction::SHR, 2 }
};

map<Instruction, string> Assembler::instrOpCode = {
    { Instruction::HALT, "00" },
    { Instruction::IRET, "08" },
    { Instruction::RET, "10" },
    { Instruction::INT, "18" },
    { Instruction::CALL, "20" },
    { Instruction::JMP, "28" },
    { Instruction::JEQ, "30" },
    { Instruction::JNE, "38" },
    { Instruction::JGT, "40" },
    { Instruction::PUSH, "48" },
    { Instruction::POP, "50" },
    { Instruction::XCHG, "58" },
    { Instruction::MOV, "60" },
    { Instruction::ADD, "68" },
    { Instruction::SUB, "70" },
    { Instruction::MUL, "78" },
    { Instruction::DIV, "80" },
    { Instruction::CMP, "88" },
    { Instruction::NOT, "90" },
    { Instruction::AND, "98" },
    { Instruction::OR, "A0" },
    { Instruction::XOR, "A8" },
    { Instruction::TEST, "B0" },
    { Instruction::SHL, "B8" },
    { Instruction::SHR, "C0" }
};

map<string, Instruction> Assembler::instrName = {
    { "halt", Instruction::HALT },
    { "iret", Instruction::IRET },
    { "ret", Instruction::RET },
    { "int", Instruction::INT },
    { "call", Instruction::CALL },
    { "jmp", Instruction::JMP },
    { "jeq", Instruction::JEQ },
    { "jne", Instruction::JNE },
    { "jgt", Instruction::JGT },
    { "push", Instruction::PUSH },
    { "pop", Instruction::POP },
    { "xchg", Instruction::XCHG },
    { "mov", Instruction::MOV },
    { "add", Instruction::ADD },
    { "sub", Instruction::SUB },
    { "mul", Instruction::MUL },
    { "div", Instruction::DIV },
    { "cmp", Instruction::CMP },
    { "not", Instruction::NOT },
    { "and", Instruction::AND },
    { "or", Instruction::OR },
    { "xor", Instruction::XOR },
    { "test", Instruction::TEST },
    { "shl", Instruction::SHL },
    { "shr", Instruction::SHR }
};

map<SectionType, string> Assembler::sectionCode = {
    { SectionType::START, ".start" },
    { SectionType::TEXT, ".text" },
    { SectionType::DATA, ".data" },
    { SectionType::BSS, ".bss" },
    { SectionType::RODATA, ".rodata" },
    { SectionType::UND, ".und" }
};

map<OperandType, regex> Assembler::opTypeRgx = {
	{ op_dec, regex("(\\$)([0-9]+)") },											// $<literal>			neposredna vrednost
	{ op_sym_val, regex("(\\$)([a-zA-Z_][a-zA-Z0-9]*)") }, 						// $<simbol>			neposredna vrednost
	{ op_reg, regex("(%psw)|(%r)([0-7])([lhLH]?)|(%sp)|(%pc)") },							// %r<num>     			vrednost iz registra
	{ op_reg_ind, regex("(\\()(%r)([0-7])(\\))") },								// (%r<num>)   			vr iz memorije na adresi iz registra
	{ op_reg_ind_val, regex("([0-9]+)(\\()(%r)([0-7])(\\))") },					// <literal>(%r<num>)   vr iz memorije na adresi <literal> + r<num>
	{ op_reg_ind_sym, regex("([a-zA-Z_][a-zA-Z0-9]*)(\\()(%r)([0-6])(\\))") },  // <simbol>(%r<num>)    vr iz memorije na adresi <simbol> + r<num>
	{ op_pcrel, regex("([a-zA-Z_][a-zA-Z0-9]*)(\\()(%r7|%pc)(\\))") },   		// <literal>(%pc/%r7)   vr iz memorije na adresi <simbol>(PCrel) 
	{ op_mem, regex("([0-9]+)") },												// <literal>    		vr iz memorije na adresi <literal>
	{ op_sym_mem, regex("([a-zA-Z_][a-zA-Z0-9]*)") },							// <simbol>     		vr iz memorije na adresi <simbol>
	// jmp
	{ jmp_op_dec, regex("([0-9]+)") },											    	// <literal>			skok na adresu
	{ jmp_op_sym_val, regex("([a-zA-Z_][a-zA-Z0-9]*)") }, 								// <simbol>				skok na adresu
	{ jmp_op_reg, regex("(\\*)((%psw)|(%r)([0-7])([lhLH]?))") },						// *%r<num>     		skok na adresu iz registra
	{ jmp_op_reg_ind, regex("(\\*)(\\()(%r)([0-7])(\\))") },						 	// *(%r<num>)   	 	skok na adresu iz memorije na adresi iz registra
	{ jmp_op_reg_ind_val, regex("(\\*)([0-9]+)(\\()(%r)([0-7])(\\))") },			 	// *<literal>(%r<num>)  skok na adresu iz memorije na adresi <literal> + r<num>
	{ jmp_op_reg_ind_sym, regex("(\\*)([a-zA-Z_][a-zA-Z0-9]*)(\\()(%r)([0-6])(\\))") }, // *<simbol>(%r<num>)   skok na adresu iz memorije na adresi <simbol> + r<num>
	{ jmp_op_pcrel, regex("(\\*)([a-zA-Z_][a-zA-Z0-9]*)(\\()(%r7|%pc)(\\))") },    	 	// *<literal>(%pc/%r7)  skok na adresu iz memorije na adresi <simbol>(PCrel) 
	{ jmp_op_mem, regex("(\\*)([0-9]+)") },												// *<literal>    		skok na adresu iz memorije na adresi <literal>
	{ jmp_op_sym_mem, regex("(\\*)([a-zA-Z_][a-zA-Z0-9]*)") }							// *<simbol>     		skok na adresu iz memorije na adresi <simbol>
};

Assembler::Assembler(ifstream& in, ofstream& out): outputFile(out), locationCnt(0), jmpFlag(false) {
    parseInput(in);
}

Assembler::~Assembler(){ }

void Assembler::compile(){

    currSection = START;
    addSymbol(sectionCode[UND], UND, locationCnt, LOCAL, SECTION, 0, true);
	string textLabel = "", rodataLabel = "";

    for(auto& lineQ: asmInput){

        currToken = (TokenType) tokenParser(lineQ.front());
        string tokenName;

        if(currToken == LABEL){
            tokenName = lineQ.front().substr(0, lineQ.front().size() - 1);
			if (textLabel != "" && currSection == TEXT && textLabel != tokenName)
				symbolTable.find(textLabel)->second.size = locationCnt - symbolTable.find(textLabel)->second.offset;
			if (currSection == TEXT) textLabel = tokenName;
			if (currSection == RODATA) rodataLabel = tokenName;
            lineQ.pop();

			if (symbolTable.find(tokenName) != symbolTable.end()) {
				updateSymbol(tokenName, currSection, locationCnt, currToken, true);
				for (int i = 0; i < symbolTable.find(tokenName)->second.flink.size(); ++i) {
					if (symbolTable.find(tokenName)->second.flink[i].patch < 0) { //equ
						string equName = symbolTable.find(tokenName)->second.flink[i].name;
						int sign = symbolTable.find(tokenName)->second.flink[i].equ_sign;
						symbolTable.find(equName)->second.offset += sign * locationCnt;
						if(--symbolTable.find(equName)->second.size)
							symbolTable.find(equName)->second.defined = true;
					}
					else {
						int j = 0, old_j = 0, offset = -1;
						string bytesStr = ""; Section* section = 0;
						int relocIndex = 0;
						while (j < relocations.size())
							if (relocations[j++].name == tokenName && relocations[j - 1].offset == symbolTable.find(tokenName)->second.flink[i].patch) {
								offset = relocations[--j].offset;
								section = &(sections[relocations[j].section]);
								relocIndex = j;
								break;
							}
						if (symbolTable.find(tokenName)->second.scope == GLOBAL) continue;
						if (offset == -1) {
							cout << "Error - relocation.\n"; exit(3);
						}
						for (int i = 0, j = 0; i < section->size; i++) {
							if (j >= offset) break;
							if (section->content.find(i) != section->content.end()) {
								bytesStr = section->content.find(i)->second;
								old_j = j;
								j += bytesStr.length() / 2;
							}
							else continue;
						}
						offset -= old_j;
						string replacement = decToHex(locationCnt, 2);	
						bytesStr.replace(offset * 2, 4, replacement.substr(2, 2) + replacement.substr(0, 2));
						section->content.find(old_j)->second = bytesStr;
						if (section->name == sectionCode[symbolTable.find(tokenName)->second.section])
							relocations.erase(relocations.begin() + relocIndex);
					}
				}
				symbolTable.find(tokenName)->second.flink.clear();
				
			}
            else addSymbol(tokenName, currSection, locationCnt, LOCAL, currToken, 0, true);

            if(lineQ.empty()) continue;
        }

        tokenName = lineQ.front();
        currToken = (TokenType) tokenParser(tokenName);
        lineQ.pop();

        switch (currToken)
		{
		case LABEL:
			cout << "Double label definition in the same row." << endl;
            exit(1);
			break;
		case DIRECTIVE:
			directiveHandler(tokenName, lineQ, rodataLabel);
			break;
		case SECTION:
			tokenName = lineQ.front();
			lineQ.pop();
			if(regex_match(tokenName, regex("(\\.)(text|data|bss|rodata)")))
				sections.insert({ tokenName, Section(tokenName, locationCnt) }); 
			if (currSection != START) {
				sections.find(sectionCode[currSection])->second.size = locationCnt;
				symbolTable.find(sectionCode[currSection])->second.size = locationCnt;
			}
			//updateSection
            locationCnt = 0;
            for(auto i: sectionCode){
                if(i.second == tokenName){
                    currSection = i.first; 
                    break;
                }
			}
			if(symbolTable.find(tokenName) == symbolTable.end()) addSymbol(tokenName, currSection, locationCnt, LOCAL, currToken, 0, true);
			else updateSymbol(tokenName, currSection, locationCnt, currToken, true);
			break;
		case EXT_GLB:
			while (!lineQ.empty()) {
				tokenName = lineQ.front();
				lineQ.pop();
				if (symbolTable.find(tokenName) != symbolTable.end())
					symbolTable.find(tokenName)->second.scope = GLOBAL;
				else
					addSymbol(tokenName, UND, 0, GLOBAL, SYMBOL, 0, false);
			}
			break;
		case INSTRUCTION:
			if (currSection != TEXT){
				cout << "Instructions can't be defined outside of text section." << endl;
                exit(1);
            }
			instructionHandler(tokenName, lineQ);
			break;
		case END:
			sections.insert({ tokenName, Section(tokenName, locationCnt) });
			if (currSection != START) {
				sections.find(sectionCode[currSection])->second.size = locationCnt;
				symbolTable.find(sectionCode[currSection])->second.size = locationCnt;
			}

			if (textLabel != "" && currSection == TEXT && textLabel != tokenName)
				symbolTable.find(textLabel)->second.size = locationCnt - symbolTable.find(textLabel)->second.offset;
			if (currSection == TEXT) textLabel = tokenName;
			break;
		default:
			cout << "Wrong token." << endl;
            exit(1);
		}
    }
	for (auto& it : symbolTable)
		if (!it.second.defined) it.second.scope = GLOBAL; 

	// simboli:
	outputFile << "  LABEL    SECTION    OFFSET    SCOPE    S.N." << endl;
	for (int i = 0; i < symbols.size(); ++i) {
		outputFile << "  " << setfill(' ') << setw(9) << left << symbolTable.find(symbols[i])->second.label;
		outputFile << setfill(' ') << setw(13) << sectionCode[symbolTable.find(symbols[i])->second.section];
		outputFile << setfill(' ') << setw(8) << hex << symbolTable.find(symbols[i])->second.offset;
		outputFile << setfill(' ') << setw(10) << ((symbolTable.find(symbols[i])->second.scope == GLOBAL) ? "global" : "local");
		outputFile << setfill(' ') << setw(6) << symbolTable.find(symbols[i])->second.serialNum << endl;
	}
	// sekcije
	if (sections.find(sectionCode[TEXT]) != sections.end()) {
		outputFile << endl << endl << "  #.text" << endl;
		for (int i = 0, j = 0; i < sections[sectionCode[TEXT]].size; i++) {
			if (sections[sectionCode[TEXT]].content.find(i) != sections[sectionCode[TEXT]].content.end()) {
				string bytes = sections[sectionCode[TEXT]].content.find(i)->second;
				outputFile << setfill(' ') << setw(3) << right<< hex << j << ":  ";
				for (int k = 0; k < bytes.length(); k++) {
					if ((k != 0) && (k % 2 == 0)) outputFile << " ";
					outputFile << bytes[k];
				}
				j += bytes.length() / 2;
				outputFile << endl;
			}
			else continue;
		}
	}
	if (sections.find(sectionCode[DATA]) != sections.end()) {
		outputFile << endl << "  #.data" << endl << " ";
		for (int i = 0; i < sections[sectionCode[DATA]].size; i++) {
			if (sections[sectionCode[DATA]].content.find(i) != sections[sectionCode[DATA]].content.end()) {
				string bytes = sections[sectionCode[DATA]].content.find(i)->second;
				for (int k = 0; k < bytes.length(); k++) {
					if ((k != 0) && (k % 2 == 0)) outputFile << " ";
					outputFile << bytes[k];
				}
				outputFile << " ";
			}
			else continue;
		}
		outputFile << endl;
	}
	if (sections.find(sectionCode[RODATA]) != sections.end()) {
		outputFile << endl << "  #.rodata" << endl << " ";
		for (int i = 0; i < sections[sectionCode[RODATA]].size; i++) {
			if (sections[sectionCode[RODATA]].content.find(i) != sections[sectionCode[RODATA]].content.end()) {
				string bytes = sections[sectionCode[RODATA]].content.find(i)->second;
				for (int k = 0; k < bytes.length(); k++) {
					if ((k != 0) && (k % 2 == 0)) outputFile << " ";
					outputFile << bytes[k];
				}
				outputFile << " ";
			}
			else continue;
		}
		outputFile << endl;
	}
	// relokacije:
	int offs = 0, sn = 0;
	outputFile << endl << endl << "  #.rel.text" << endl;
	for (int i = 0; i < relocations.size(); ++i) {
		if (relocations[i].section != ".text") continue;
		if (symbolTable.find(relocations[i].name)->second.scope == LOCAL && relocations[i].type == PCREL)
			offs = symbolTable.find(relocations[i].name)->second.offset;
		else offs = relocations[i].offset;
		if (symbolTable.find(relocations[i].name)->second.scope == GLOBAL)
			sn = symbolTable.find(relocations[i].name)->second.serialNum;
		else sn = symbolTable.find(sectionCode[symbolTable.find(relocations[i].name)->second.section])->second.serialNum;
		outputFile << " " << setfill('0') << setw(8) << hex << offs;
		outputFile << setfill(' ') << setw(16) << ((relocations[i].type == ABS) ? "R_x86_64_32" : "R_x86_64_PC32");
		outputFile << setfill(' ') << setw(5) << dec << sn;
		//outputFile << setfill(' ') << setw(5) << dec << relocations[i].addend;
		outputFile << endl;
	}
	outputFile << endl << endl << "  #.rel.data" << endl;
	for (int i = 0; i < relocations.size(); ++i) {
		if (relocations[i].section != ".data") continue;
		outputFile << " " << setfill('0') << setw(8) << hex << relocations[i].offset;
		outputFile << setfill(' ') << setw(16) << ((relocations[i].type == ABS) ? "R_x86_64_32" : "R_x86_64_PC32");
		outputFile << setfill(' ') << setw(5) << dec << symbolTable.find(relocations[i].name)->second.serialNum;
		//outputFile << setfill(' ') << setw(5) << dec << relocations[i].addend;
		outputFile << endl;
	}
	outputFile << endl;
	
}

void Assembler::parseInput(ifstream& in){
    string line;
    while(getline(in, line)){
        // //line.erase(line.length() - 1);
        queue<string> tokens;

        size_t start = line.find_first_not_of(" ,\t");
        size_t end = start;
        while(start != string::npos){     // 'until the end of the string'
            end = line.find_first_of(" ,\t", start);
            tokens.push(line.substr(start, end - start));
            start = line.find_first_not_of(" ,\t", end);
        }

        if(tokens.size() == 0) continue;
        if(tokens.front() == ".end"){
            asmInput.push_back(tokens);
            break;
        }

        asmInput.push_back(tokens);
    }
}

void Assembler::addSymbol(string label, SectionType sec, int offs, ScopeType scp, TokenType tok, int size, bool def){
    
    symbolTable.insert({ label, Symbol(label, sec, offs, scp, tok, size, def) });
	symbols.push_back(label);

}

void Assembler::updateSymbol(string label, SectionType currSection, int locationCnt, TokenType currToken, bool def) {
	symbolTable.find(label)->second.section = currSection;
	symbolTable.find(label)->second.offset = locationCnt;
	symbolTable.find(label)->second.symType = currToken;
	if (currToken == SECTION) symbolTable.find(label)->second.scope = LOCAL;
	symbolTable.find(label)->second.defined = def;
}

TokenType Assembler::tokenParser(string token){
    TokenType tokType = INCORRECT;

	regex label{ "([a-zA-Z][a-zA-Z0-9_]*:)" };
	regex directive{ "\\.(byte|word|skip|equ)" };
	regex instruction{ "(halt|iret|ret|int|call|jmp|jeq|jne|jgt|push|pop|xchg|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr)?" };
	regex symbol{ ("([a-zA-Z_][a-zA-Z0-9]*)") };
	regex op_dec{ ("(\\$)([0-9]+)") };
	regex end{ "(\\.)(end)" };

	if (!token.compare(".global") || !token.compare(".extern"))
		return  EXT_GLB;
    if (regex_match(token, label))
		 return LABEL;
	 if (regex_match(token, directive))
		return DIRECTIVE;
	 if (!token.compare(".section"))
		 return SECTION;
	 if (regex_match(token, instruction))
		 return INSTRUCTION;
	 if (regex_match(token, symbol))
		 return SYMBOL;
	 if (regex_match(token, op_dec))
		 return OP_DEC;
	 if (regex_match(token, end))
		 return END;

	return tokType;
}

string Assembler::decToHex(int dec, int numOfBytes){
	stringstream ss;
	ss << setfill('0') << setw(2*numOfBytes) << hex << dec;
	string hexx(ss.str());
	for(unsigned i = 0; i < hexx.length(); ++i)
		if(isalpha(hexx[i])) hexx[i] = toupper(hexx[i]); 
	return hexx;
}

void Assembler::directiveHandler(string dir, queue<string>& tokens, string& label){
    
    string byteStr;
	int value = 0;
	int helpInt = 0;

	if (dir == ".equ") {
		string name = tokens.front();
		tokens.pop();
		TokenType opType = (TokenType)tokenParser(name);
		if (opType != SYMBOL) {
			cout << "Directive .equ needs symbol as first operand." << endl;
			exit(1);
		}
		string equ = "", op = "";
		while (!tokens.empty()) {
			equ += tokens.front();
			tokens.pop();
		}
		equ += '\0';
		string helpEqu = "";
		helpInt = 0;
		//equ.erase(remove_if(equ.begin(), equ.end(), isspace), equ.end());

		for(; helpInt < equ.length(); ++helpInt)
			if(equ[helpInt] != ' ') helpEqu += equ[helpInt];
		equ = helpEqu;	 
		helpInt = 0;
		bool neg = false, defined = true;
		value = 0;
		if (equ[helpInt] == '-') {
			neg = true;
			++helpInt;
		}
		while ((unsigned)helpInt < equ.length()) {
			while (equ[helpInt] != '\0' && equ[helpInt] != '+' && equ[helpInt] != '-')
				op += equ[helpInt++];

			if (strspn(op.c_str(), "0123456789") == op.size()) 
				value += (neg ? (0 - atoi(op.c_str())) : atoi(op.c_str()));
			else if (symbolTable.find(op) != symbolTable.end()) {
				Symbol* symbol = &(symbolTable.find(op)->second);
				if (symbol->defined) value += (neg ? (0 - symbol->offset) : symbol->offset);
				else {
					defined = false;
					symbol->size++;
					symbol->flink.push_back(forw_ref(-1, name, (neg ? -1 : 1)));
				}
			}
			else {
				defined = false;
				addSymbol(op, UND, 0, LOCAL, SYMBOL, 0, false);
				symbolTable.find(op)->second.size++;
				symbolTable.find(op)->second.flink.push_back(forw_ref(-1, name, (neg ? -1 : 1)));
			}
			op = "";
			if (equ[helpInt++] == '-') neg = true;
			else neg = false;
		}
		if (symbolTable.find(name) != symbolTable.end()) {
			symbolTable.find(name)->second.section = currSection;
			symbolTable.find(name)->second.offset = value;
			updateSymbol(name, currSection, value, opType, defined);
			for (int i = 0; defined && (i < symbolTable.find(name)->second.flink.size()); ++i) {
				if (symbolTable.find(name)->second.flink[i].patch < 0) {
					string equName = symbolTable.find(name)->second.flink[i].name;
					int sign = symbolTable.find(name)->second.flink[i].equ_sign;
					symbolTable.find(equName)->second.offset += sign * locationCnt;
					if (--symbolTable.find(equName)->second.size)
						symbolTable.find(equName)->second.defined = true;
				}
				else {
					int j = 0, old_j = 0, offset = -1, relocIndex = 0;
					string bytesStr = "";
					while (j < relocations.size())
						if (relocations[j++].name == name) {
							offset = relocations[--j].offset;
							relocIndex = j;
							break;
						}
					if (offset == -1) {
						cout << "Error - relocation.\n"; exit(3);
					}
					for (int i = 0, j = 0; i < sections[sectionCode[symbolTable.find(name)->second.section]].size; i++) {
						if (j >= offset) break;
						if (sections[sectionCode[symbolTable.find(name)->second.section]].content.find(i)
							!= sections[sectionCode[symbolTable.find(name)->second.section]].content.end()) {
							bytesStr = sections[sectionCode[symbolTable.find(name)->second.section]].content.find(i)->second;
							old_j = j;
							j += bytesStr.length() / 2;
						}
						else continue;
					}
					offset -= old_j;
					string replacement = decToHex(locationCnt, 2);
					bytesStr.replace(offset * 2, 4, replacement);
					sections[sectionCode[symbolTable.find(name)->second.section]].content.find(old_j)->second = bytesStr;
					relocations.erase(relocations.begin() + relocIndex);
				}
			}

		}
		else
			addSymbol(name, UND, value, LOCAL, SYMBOL, 0, defined);
	}

	if (dir == ".skip"){
		string op = tokens.front();  
		tokens.pop();
		if (!regex_match(op, regex("([0-9]+)"))) {
            cout << "Directive .skip needs decimal operand." << endl;
            exit(1);
        }
		value = operandParser(op, helpInt, helpInt);
        sections[sectionCode[currSection]].writeZeroBytes(locationCnt, value);
		
		locationCnt += value;
		sections[sectionCode[currSection]].size += value;
		if (label != "" && symbolTable.find(label) != symbolTable.end()) {
			symbolTable.find(label)->second.size = locationCnt - symbolTable.find(label)->second.offset;
			label = "";
		}
		return;
	}

	if (dir == ".byte"){
        if (currSection == BSS) {
            cout << "Error: .byte directive in .bss section." << endl;
            exit(1);
        }
        while (!tokens.empty()){
			string op = tokens.front();  
			tokens.pop();
			if (regex_match(op, regex("([0-9]+)")))
				value = operandParser(op, helpInt, helpInt);
			else  value = setAbsReloc(op, locationCnt, -1);
			//write byte
			value &= 0xFF;
			byteStr = decToHex(value, 1);
			sections[sectionCode[currSection]].writeByte(locationCnt, byteStr);

			locationCnt++;
			sections[sectionCode[currSection]].size++;
        }
		return;
	}
	if (dir == ".word"){
        if (currSection == BSS) {
            cout << "Error: .word directive in .bss section." << endl;
            exit(1);
        }
		while (!tokens.empty()) {

			string op = tokens.front();
			tokens.pop();
			if (regex_match(op, regex("([0-9]+)")))
				value = operandParser(op, helpInt, helpInt);
			else value = setAbsReloc(op, locationCnt, -2);
			//write word // little endian ordering
			value &= 0xFFFF;
			string byteStr1 = decToHex(value >> 8, 1);
			string byteStr2 = decToHex(value & 0xFF, 1);
			sections[sectionCode[currSection]].writeBytes(locationCnt, byteStr2 + byteStr1);

			locationCnt += 2;
			sections[sectionCode[currSection]].size += 2;
        }
		return;
	}
}

void Assembler::instructionHandler(string instr, queue<string>& tokens){
	regex instruction{ "(halt|iret|ret|int|call|jmp|jeq|jne|jgt|push|pop|xchg|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr)?" };
	regex jmp_instruction{ "(int|call|jmp|jeq|jne|jgt)?" };
	smatch match;
	if(!regex_match(instr, match, instruction)){
		cout << "Error - Non-existent instruction." << endl;
        exit(1);
	}
	if(regex_match(instr, match, jmp_instruction)) jmpFlag = true;
	else jmpFlag = false;

	int numOfOper = instrNumOper[instrName[instr]];
	string instrByteStr = instrOpCode[instrName[instr]], op1ByteStr, op2ByteStr;
	int op1Byte, op2Byte;
	int numOfBytes1 = 0, numOfBytes2 = 0; 
	int regNum = -1, pcrel = 0, addend = 0;
	
	if (numOfOper == 0){
		if(!tokens.empty()){
			cout << "Error - Too many arguments." << endl;
            exit(1);
		}
		sections[sectionCode[currSection]].writeByte(locationCnt, instrByteStr);
		locationCnt++; // instructionDescriptor (OC_(4-0).S.Un.Un)
		sections[sectionCode[currSection]].size++;
		return;
	}

	string op1 = tokens.front(), op2 = "";
	tokens.pop();
	int am1 = addressingMode(op1), am2 = 0;
	int op1Val = operandParser(op1, numOfBytes1, regNum), op2Val = 0;
	if (numOfOper == 2) {
		if (tokens.empty()) {
			cout << "Error - Too few arguments." << endl;
			exit(1);
		}
		op2 = tokens.front();
		tokens.pop();
		if (numOfBytes2 == 2 && numOfBytes1 == 2)
			instrByteStr = decToHex((int)strtol(instrByteStr.c_str(), NULL, 16) | 4, 1);
	}
	
	op1Byte = am1 << 5;
	if (op1Val == -1) // symbol
		op1Val = setAbsReloc(op1, locationCnt + 2, -(2 + ((numOfOper == 2) ? (1 + numOfBytes2) : 0)));
	else if (op1Val == -2) {
		pcrel = 1;
		addend = -(2 + ((numOfOper == 2) ? (1 + numOfBytes2) : 0));
		op1Val = setPCrelReloc(op1, locationCnt + 2, addend);
	}

	if(!numOfBytes1){
		op1Byte |= regNum << 1;
		if(toupper(op1[op1.length() - 1]) == 'H') op1Byte |= 1;
		op1ByteStr = decToHex(op1Byte, 1);
	} else if(numOfBytes1 == 1){ // immed decimal 
		op1Byte <<= 8;
		op1Byte |= (op1Val & 0xFF);
		op1ByteStr = decToHex(op1Byte, 2);
	} else if(numOfBytes1 == 2){
		if(regNum > -1){
			op1Byte |= regNum << 1;
			op1Byte <<= 16;
			op1Byte |= (op1Val & 0xFFFF); // decimal (symbol: undefined - 0 / defined - op1Val)
			op1ByteStr = decToHex(op1Byte, 3);
		} else { 
			op1Byte <<= 16;
			op1Byte |= (op1Val & 0xFFFF); // decimal (symbol: undefined - 0 / defined - op1Val)
			op1ByteStr = decToHex(op1Byte, 3);
		}
	} else exit(2);

	if (pcrel) {
		string temp = decToHex((0x10000 + addend), 2);
		op1ByteStr = op1ByteStr.substr(0, 2) + temp.substr(2, 2) + temp.substr(0, 2);
	}

	if (numOfOper == 1){
		if(!tokens.empty()){
			cout << "Error - Too many arguments." << endl;
            exit(1);
		}
		if((instrName[match[1]] != PUSH) && (am1 == 0)){
			cout << "Error - Invalid addressing mode (immediate) for destination operand." << endl;
			exit(1);
		}

		sections[sectionCode[currSection]].writeBytes(locationCnt, instrByteStr + op1ByteStr);
		locationCnt += 2 + numOfBytes1; // 2 = (InstrDescr + Op1Descr)
		sections[sectionCode[currSection]].size += 2 + numOfBytes1;
		return;
	}

	regNum = -1; pcrel = 0; addend = 0;
	am2 = addressingMode(op2);
	op2Val = operandParser(op2, numOfBytes2, regNum);

	if(!tokens.empty()){
		cout << "Error - Too many arguments." << endl;
		exit(1);
	}
	if(((instrName[match[1]] != SHR) && (am2 == 0)) || ((instrName[match[1]] == SHR) && (am1 == 0))){
		cout << "Error - Invalid addressing mode (immediate) for destination operand." << endl;
		exit(1);
	}
	
	op2Byte = am2 << 5;
	if (op2Val == -1) // symbol
		op2Val = setAbsReloc(op2, locationCnt + 3 + numOfBytes1, -2);
	else if (op2Val == -2) {
		pcrel = 1; 
		addend = -2;
		op2Val = setPCrelReloc(op2, locationCnt + 3 + numOfBytes1, addend);
	}

	if(!numOfBytes2){
		op2Byte |= regNum << 1;
		if(toupper(op2[op2.length() - 1]) == 'H') op2Byte |= 1;
		op2ByteStr = decToHex(op2Byte, 1);
	} else if(numOfBytes2 == 1){ // immed decimal
		op2Byte <<= 8;
		op2Byte |= (op2Val & 0xFF);
		op2ByteStr = decToHex(op2Byte, 2);
	} else if(numOfBytes2 == 2){
		if(regNum > -1){
			op2Byte |= regNum << 1;
			op2Byte <<= 16;
			op2Byte |= (op2Val & 0xFFFF); // decimal (symbol: undefined - 0 / defined - op2Val)
			op2ByteStr = decToHex(op2Byte, 3);
		} else { 
			op2Byte <<= 16;
			op2Byte |= (op2Val & 0xFFFF); // decimal (symbol: undefined - 0 / defined - op2Val)
			op2ByteStr = decToHex(op2Byte, 3);
		}
	} else exit(2);

	if (pcrel) {
		string temp = decToHex((0x10000 + addend), 2);
		op2ByteStr = op2ByteStr.substr(0, 2) + temp.substr(2, 2) + temp.substr(0, 2);
	}

	sections[sectionCode[currSection]].writeBytes(locationCnt, instrByteStr + op1ByteStr + op2ByteStr);
	locationCnt += 3 + numOfBytes1 + numOfBytes2; // 3 = (InstrDescr + Op1Descr + Op2Descr)
	sections[sectionCode[currSection]].size += 3 + numOfBytes1 + numOfBytes2;

}

int Assembler::addressingMode(string operand){
    smatch match;

	if(jmpFlag){
		if (regex_match(operand, match, opTypeRgx[jmp_op_dec]) 
			|| regex_match(operand, match, opTypeRgx[jmp_op_sym_val])) return 4; 	  // memdir
    
		if (regex_match(operand, match, opTypeRgx[jmp_op_reg])) return 1;        // regdir

		if (regex_match(operand, match, opTypeRgx[jmp_op_reg_ind])) return 2;    // regind

		if (regex_match(operand, match, opTypeRgx[jmp_op_reg_ind_val])
			|| regex_match(operand, match, opTypeRgx[jmp_op_reg_ind_sym])
			|| regex_match(operand, match, opTypeRgx[jmp_op_pcrel])) return 3;   // regindpom

		if (regex_match(operand, match, opTypeRgx[jmp_op_mem])
			|| regex_match(operand, match, opTypeRgx[jmp_op_sym_mem])) return 4; // memind
	}

    if (regex_match(operand, match, opTypeRgx[op_dec]) 
		|| regex_match(operand, match, opTypeRgx[op_sym_val])) return 0; // immed
    
    if (regex_match(operand, match, opTypeRgx[op_reg])) return 1;        // regdir

    if (regex_match(operand, match, opTypeRgx[op_reg_ind])) return 2;    // regind

    if (regex_match(operand, match, opTypeRgx[op_reg_ind_val])
		|| regex_match(operand, match, opTypeRgx[op_reg_ind_sym])
		|| regex_match(operand, match, opTypeRgx[op_pcrel])) return 3;   // regindpom

	if (regex_match(operand, match, opTypeRgx[op_mem])
		|| regex_match(operand, match, opTypeRgx[op_sym_mem])) return 4; // mem
   
	cout << "Error: Non-existent addressing type." << endl;
	exit(1);

}

int Assembler::operandParser(string& operand, int& numOfBytes, int& reg){
	smatch match;

	if(jmpFlag){
		// memdir
		if (regex_match(operand, match, opTypeRgx[jmp_op_dec])){
			numOfBytes = (stoi(match[1]) > 0xFF) ? 2 : 1; 
			return stoi(match[1]);
		}
		if (regex_match(operand, match, opTypeRgx[jmp_op_sym_val])){
			numOfBytes = 2; 
			operand = match[1];
			return -1; // setAbsReloc(match[1]);
		}
		// regdir
		if (regex_match(operand, match, opTypeRgx[jmp_op_reg])){
			numOfBytes = 0;
			if (operand == "%psw") reg = 15;
			else if (operand == "%sp") reg = 6;
			else if (operand == "%pc") reg = 7;
			else reg = stoi(match[5]);
			return 0;
		}
		// regind
		if (regex_match(operand, match, opTypeRgx[jmp_op_reg_ind])){
			numOfBytes = 0;
			reg = stoi(match[4]);
			return 0;
		}
		// regindpom
		if (regex_match(operand, match, opTypeRgx[jmp_op_reg_ind_val])){
			numOfBytes = 2;
			reg = stoi(match[5]);
			return stoi(match[2]); 
		}
		if (regex_match(operand, match, opTypeRgx[jmp_op_reg_ind_sym])){
			numOfBytes = 2;
			reg = stoi(match[5]);
			operand = match[2];
			return -1;// setAbsReloc(match[2]);
		}
		if (regex_match(operand, match, opTypeRgx[jmp_op_pcrel])){
			numOfBytes = 2;
			reg = 7; // pc
			operand = match[2];
			return -2; // setPCrelReloc(match[2]);
		}
		// memind
		if (regex_match(operand, match, opTypeRgx[jmp_op_mem])){
			numOfBytes = 2;
			return stoi(match[2]);
		}
		if (regex_match(operand, match, opTypeRgx[jmp_op_sym_mem])){
			numOfBytes = 2;
			operand = match[2];
			return -1; // setAbsReloc(operand);
		}

		cout << "Error - Operand type is not recognized." << endl;
		exit(1);
		return 0;
	}

	// immed
	if (regex_match(operand, match, opTypeRgx[op_dec])){
		numOfBytes = (stoi(match[2]) > 0xFF) ? 2 : 1; 
		return stoi(match[2]);
	}
	if (regex_match(operand, match, opTypeRgx[op_sym_val])){
		numOfBytes = 2;
		operand = match[2];
		return -1; // setAbsReloc(match[2]);
	}
	// regdir
	if (regex_match(operand, match, opTypeRgx[op_reg])){
		numOfBytes = 0;
		if (operand == "%psw") reg = 15;
		else if (operand == "%sp") reg = 6;
		else if (operand == "%pc") reg = 7;
		else reg = stoi(match[3]);
		return 0;
	}
	// regind
	if (regex_match(operand, match, opTypeRgx[op_reg_ind])){
		numOfBytes = 0;
		reg = stoi(match[3]);
		return 0;
	}
	// regindpom
	if (regex_match(operand, match, opTypeRgx[op_reg_ind_val])){
		numOfBytes = 2;
		reg = stoi(match[4]);
		return stoi(match[1]); 
	}
	if (regex_match(operand, match, opTypeRgx[op_reg_ind_sym])){
		numOfBytes = 2;
		reg = stoi(match[4]);
		operand = match[1];
		return -1; // setAbsReloc(match[1]);
	}
	if (regex_match(operand, match, opTypeRgx[op_pcrel])){
		numOfBytes = 2;
		reg = 7; // pc
		operand = match[1];
		return -2; // setPCrelReloc(match[1]);
	}
	// mem
	if (regex_match(operand, match, opTypeRgx[op_mem])){
		numOfBytes = 2;
		return stoi(match[1]);
	}
	if (regex_match(operand, match, opTypeRgx[op_sym_mem])){
		numOfBytes = 2;
		return -1; // setAbsReloc(operand);
	}

	cout << "Error - Operand type is not recognized." << endl;
	exit(1);
	return 0;
}

int Assembler::setAbsReloc(string symbolStr, int offset, int addend){
	
	auto symbol = symbolTable.find(symbolStr);
																				
	if (symbol == symbolTable.end()) { 
		addSymbol(symbolStr, UND, 0, LOCAL, SYMBOL, 0, false);
		symbolTable.find(symbolStr)->second.flink.push_back(forw_ref(offset));
		relocations.push_back(Reloc(symbolStr, sectionCode[currSection], offset, ABS, addend));
	}
	else { 
		if (symbol->second.defined) {
			relocations.push_back(Reloc(symbolStr, sectionCode[currSection], offset, ABS, addend));
			return symbolTable.find(symbolStr)->second.offset;
		}
		else {
			relocations.push_back(Reloc(symbolStr, sectionCode[currSection], offset, ABS, addend));
			symbolTable.find(symbolStr)->second.flink.push_back(forw_ref(offset));
		}
	}
	
	return 0;
}

int Assembler::setPCrelReloc(string symbolStr, int offset, int addend){
	
	auto symbol = symbolTable.find(symbolStr);

	if(symbol == symbolTable.end()){
		addSymbol(symbolStr, UND, 0, LOCAL, SYMBOL, 0, false);
		symbolTable.find(symbolStr)->second.flink.push_back(forw_ref(offset));
		relocations.push_back(Reloc(symbolStr, sectionCode[currSection], offset, PCREL, addend));
	} 
	else {
		if(symbol->second.defined){ 
			relocations.push_back(Reloc(symbolStr, sectionCode[currSection], offset, PCREL, addend));
			return symbolTable.find(symbolStr)->second.offset;
		}
		else {
			relocations.push_back(Reloc(symbolStr, sectionCode[currSection], offset, PCREL, addend));
			symbolTable.find(symbolStr)->second.flink.push_back(forw_ref(offset));
		}
	}
	
	return 0;
}
