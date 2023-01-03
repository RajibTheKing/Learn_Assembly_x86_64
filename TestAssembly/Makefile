NASM=nasm
NASMFILES=utility_functions.asm
GASFILES=$(shell echo ./assembly/*.s| tr ' ' '\n')

CPPFILES=$(shell echo ./*.cpp| tr ' ' '\n')

OBJFILES=$(shell echo ./*.o| tr ' ' '\n') \
         $(shell echo ./assembly/*.o | tr ' ' '\n')

TARGET=test_assembly

all:
	# $(NASM) -f elf64 -g -F dwarf $(NASMFILES)
	@nice ./dependencies.sh $(TARGET) $(GASFILES) $(CPPFILES)
	
clean:
	rm -rf $(OBJFILES) $(TARGET) *~
