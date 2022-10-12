CC=g++
ASMCOMPILER=nasm
ASMFILES=utility_functions.asm
CPPFILES=assembly_wrapper.cpp main.cpp
OBJFILES=assembly_wrapper.o main.o
TARGET=test_assembly

all:
	$(CC) -c $(CPPFILES)
	$(ASMCOMPILER) -f elf64 $(ASMFILES)
	$(CC) -o $(TARGET) $(OBJFILES) utility_functions.o
	
clean:
	rm -f $(OBJFILES) $(TARGET) *~