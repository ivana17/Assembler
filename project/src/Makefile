OBJ = assembler.cpp main.cpp symbol.cpp reloc.cpp section.cpp
prog: $(OBJ)
	g++ -std=c++11 -gdwarf-2 $(OBJ) -o assembler
clean:
	rm *^(\.cpp$|\.h$) assembler
	