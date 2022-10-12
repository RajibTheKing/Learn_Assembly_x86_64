CC=g++
NASM=nasm
NASMFILES=utility_functions.asm
GASFILES=utility_functions_GAS.s
CPPFILES=assembly_wrapper.cpp main.cpp
OBJFILES=assembly_wrapper.o main.o utility_functions_GAS.o utility_functions.o
TARGET=test_assembly

all:
	$(CC) -c $(CPPFILES) $(GASFILES)
	$(NASM) -f elf64 $(NASMFILES)
	$(CC) -o $(TARGET) $(OBJFILES)
	
clean:
	rm -f $(OBJFILES) $(TARGET) *~