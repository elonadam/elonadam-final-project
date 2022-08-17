flags = -Wall -ansi -pedantic -g

all: assembler

assembler: assembler.o guides.o io.o pass1.o pass2.o preasm.o supfuncs.o table.o error.o memoryList.o globals.o instructs.o
	gcc $(flags) assembler.o guides.o io.o pass1.o pass2.o preasm.o supfuncs.o table.o error.o memoryList.o globals.o instructs.o -o assembler
	
assembler.o: assembler.c assembler.h table.h error.h
	gcc $(flags) -c assembler.c -o assembler.o
	
guides.o: guides.c guides.h
	gcc $(flags) -c guides.c -o guides.o
	
io.o: io.c io.h error.h
	gcc $(flags) -c io.c -o io.o
	
pass1.o: pass1.c table.h io.h memoryList.h error.h guides.h supfuncs.h instructs.h
	gcc $(flags) -c pass1.c -o pass1.o
	
pass2.o: pass2.c pass2.h io.h supfuncs.h memoryList.h guides.h error.h table.h instructs.h
	gcc $(flags) -c pass2.c -o pass2.o
	
preasm.o: preasm.c preasm.h io.h supfuncs.h table.h error.h
	gcc $(flags) -c preasm.c -o preasm.o
	
supfuncs.o: supfuncs.c io.h preasm.h supfuncs.h table.h guides.h error.h instructs.h memoryList.h
	gcc $(flags) -c supfuncs.c -o supfuncs.o
	
table.o: table.c table.h io.h error.h supfuncs.h
	gcc $(flags) -c table.c -o table.o
	
error.o: error.c error.h io.h memoryList.h
	gcc $(flags) -c error.c -o error.o

memoryList.o: memoryList.c memoryList.h error.h io.h
	gcc $(flags) -c memoryList.c -o memoryList.o

globals.o: globals.c guides.h instructs.h io.h
	gcc $(flags) -c globals.c -o globals.o

instructs.o: instructs.c instructs.h
	gcc $(flags) -c instructs.c -o instructs.o

clean:
	rm *.o
