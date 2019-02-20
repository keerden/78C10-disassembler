

CPP      = g++ 
CC       = gcc 

OBJDIR   = build
$(shell mkdir -p $(OBJDIR) >/dev/null)

DEPDIR 	 = .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@


LIBS     = -static-libgcc -g3
BIN      = disassembler

CXXFLAGS = $(DEPFLAGS) -std=c++11 -g3 -std=c++11 

RM       = rm -f

SRC=$(wildcard *.cpp)

OBJS= $(addprefix $(OBJDIR)/,$(SRC:.cpp=.o))

.PHONY: all clean clean-all test


all: $(BIN)

$(BIN): $(OBJS) 
	$(CPP) -o $@ $(OBJS) $(LIBS)

$(OBJDIR)/%.o: %.cpp $(DEPDIR)/%.d
	$(CPP) -c $< -o $@ $(CXXFLAGS)
	$(POSTCOMPILE)

clean:
	@${RM} $(OBJS)
	@${RM} $(DEPDIR)/*

clean-all: clean
	@${RM} $(BIN) 

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRC))))