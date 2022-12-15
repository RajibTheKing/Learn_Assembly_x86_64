NASM=nasm
NASMFILES=utility_functions.asm
GASFILES=$(shell echo ./*.s| tr ' ' '\n')

CPPFILES=$(shell echo ./*.cpp| tr ' ' '\n')

OBJFILES=$(shell echo ./*.o| tr ' ' '\n')

TARGET=test_assembly

all:
	# $(NASM) -f elf64 -g -F dwarf $(NASMFILES)
	@nice ./dependencies.sh $(TARGET) $(GASFILES) $(CPPFILES)
	
clean:
	rm -f $(OBJFILES) $(TARGET) *~
