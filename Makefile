CXX=g++
SOURCEDIR=./src
CPPFLAGS=-Wall -Wpedantic -I$(SOURCEDIR)
OPT=

all: main.exe

main.exe: main.o
	$(CXX) $(CPPFLAGS) $^ -o $@

main.o: main.cpp $(SOURCEDIR)/sparsematrix.h
	$(CXX) $(CPPFLAGS) -c $< -o $@ $(OPT)

.PHONY: all clean

clean:
	rm -rf *.o *.exe
