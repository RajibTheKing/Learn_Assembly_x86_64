CC=g++ -std=c++20

CPPFILES=main.cpp

OBJFILES=main.o

TARGET=test_std_library

all:
	$(CC) -c -g -Wall $(CPPFILES)
	$(CC) -o $(TARGET) $(OBJFILES)
	
clean:
	rm -f $(OBJFILES) $(TARGET) *~
