# This makefile makes use of a CPLEXDIR definition on your .bashrc file.
# You can define it by adding the following example line at the end of .bashrc
# export CPLEXDIR='/home/lapo/cplex/cplex126'

# Compiler definition and CCACHE usage
override CXX    := $(shell command -v ccache 2> /dev/null) $(CXX)

# CPLEX includes
SYSTEM			:= x86-64_linux
LIBFORMAT		:= static_pic
CONCERTDIR 		:= $(CPLEXDIR)/concert
CPLEXLIBDIR		:= $(CPLEXDIR)/cplex/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR	:= $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CPLEX_NO_WARN	:= -Wno-ignored-attributes
MACROS			:= -DIL_STD -DLONG_MAX=0x7FFFFFFFL $(CPLEX_NO_WARN)
INC				:= -I $(CPLEXDIR)/cplex/include -I $(CONCERTDIR)/include
LDLIBS			:= -lilocplex -lconcert -lcplex -lm -lpthread -ldl
LDFLAGS			:= -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR)

# General includes
OPT				:= -march=native -Ofast -frename-registers -funroll-loops
CXXFLAGS  		:= -g -std=c++17 $(OPT) $(MACROS) $(INC)

# Compiler
SRC       := argparse.cpp scp-model.cpp main.cpp
OBJ       := $(SRC:%.cpp=build/%.o)
DEP       := $(SRC:%.cpp=deps/%.d)

NAME      := cplex

.PHONY: clean

default: $(NAME)

debug: CXX += -DDEBUG
debug: $(NAME)

deps/%.d: %.cpp
	@mkdir -p $(shell dirname $(shell readlink -m -- $(@)))
	@$(CXX) $(CXXFLAGS) -MM -MT $(@:deps/%.d=build/%.o) -o $(@) $(<)

build/%.o: %.cpp deps/%.d
	@mkdir -p $(shell dirname $(shell readlink -m -- $(@)))
	$(CXX) $(CXXFLAGS) -c -o $(@) $(<)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) $(LDLIBS) $(LDFLAGS)
	@echo Compilation finished

clean:
	$(RM) $(OBJ) $(DEP) $(NAME)

#ifneq ($(MAKECMDGOALS), clean)
-include $(DEP)
#endif
