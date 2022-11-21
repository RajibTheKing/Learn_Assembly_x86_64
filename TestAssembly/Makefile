CC=g++ -std=c++20
NASM=nasm
NASMFILES=utility_functions.asm
GASFILES=utility_functions_GAS.s \
         iCaseCompare_v1.s \
		 iCaseCompare_v2.s \
		 iCaseCompare_v3.s \
		 iCaseCompare_v4.s 

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
		iCaseCompare_v1.o \
		iCaseCompare_v2.o \
		iCaseCompare_v3.o \
		iCaseCompare_v4.o

TARGET=test_assembly

all:
	$(CC) -c -g -mrdrnd  -Wall $(CPPFILES) $(GASFILES)
	$(NASM) -f elf64 -g -F dwarf $(NASMFILES)
	$(CC) -o $(TARGET) $(OBJFILES)
	
clean:
	rm -f $(OBJFILES) $(TARGET) *~
