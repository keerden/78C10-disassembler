# Project: disassembler
# Makefile created by Dev-C++ 5.8.3

CPP      = g++ -D__DEBUG__
CC       = gcc -D__DEBUG__
OBJ      = main.o registerset.o utils.o
LINKOBJ  = main.o registerset.o utils.o
LIBS     = -static-libgcc -g3
BIN      = disassembler
CXXFLAGS = -std=c++11 -g3 -std=c++11
CFLAGS   =  -std=c++11 -g3 -std=c++11
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

main.o: main.cpp
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)

registerset.o: registerset.cpp
	$(CPP) -c registerset.cpp -o registerset.o $(CXXFLAGS)

utils.o: utils.cpp
	$(CPP) -c utils.cpp -o utils.o $(CXXFLAGS)
