CC=g++ -std=c++20
NASM=nasm
NASMFILES=utility_functions.asm
GASFILES=utility_functions_GAS.s
CPPFILES=assembly_wrapper.cpp \
		main.cpp \
		analyzesolution.cpp \
		utility.cpp

OBJFILES=assembly_wrapper.o \
		main.o \
		analyzesolution.o \
		utility.o \
		utility_functions_GAS.o \
		utility_functions.o \

TARGET=test_assembly

all:
	$(CC) -c -g -Wall $(CPPFILES) $(GASFILES)
	$(NASM) -f elf64 -g -F dwarf $(NASMFILES)
	$(CC) -o $(TARGET) $(OBJFILES)
	
clean:
	rm -f $(OBJFILES) $(TARGET) *~
