CC=g++ -std=c++20
NASM=nasm
NASMFILES=utility_functions.asm
GASFILES=$(shell echo ./*.s| tr ' ' '\n')

CPPFILES=$(shell echo ./*.cpp| tr ' ' '\n')

OBJFILES=$(shell echo ./*.o| tr ' ' '\n')

TARGET=test_assembly

all:
	$(CC) -c -g -mrdrnd  -Wall $(CPPFILES) $(GASFILES)
	$(NASM) -f elf64 -g -F dwarf $(NASMFILES)
	$(CC) -o $(TARGET) $(OBJFILES)
	
clean:
	rm -f $(OBJFILES) $(TARGET) *~
