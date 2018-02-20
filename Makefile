

CPP      = g++ 
CC       = gcc 

OBJDIR   = build/
OBJ      = main.o registerset.o utils.o
LINKOBJ  = main.o registerset.o utils.o

LIBS     = -static-libgcc -g3
BIN      = disassembler

CXXFLAGS = -std=c++11 -g3 -std=c++11
CFLAGS   =  -std=c++11 -g3 -std=c++11

RM       = rm -f

SRC=$(wildcard *.cpp)

OBJS= $(addprefix $(OBJDIR),$(SRC:.cpp=.o))

.PHONY: all clean 

all: $(BIN)

$(BIN): $(OBJS) 
	$(CPP) -o $@ $(OBJS) $(LIBS)

$(OBJDIR)%.o: %.cpp
	$(CPP) -c $< -o $@ $(CXXFLAGS)

clean:
	${RM}   $(BIN) $(OBJS) 

