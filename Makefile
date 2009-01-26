
INCLUDE=.
BIN=.
SRC=.

COMMON_FLAGS=-I$(INCLUDE)

GNUCPP=g++
GNUCPP_FLAGS=$(COMMON_FLAGS)

SUNCPP=CC
SUNCPP_FLAGS=$(COMMON_FLAGS) -library=rwtools7_std

CPP=$(GNUCPP)
CPP_FLAGS=$(GNUCPP_FLAGS)

BASIC_DEP= $(INCLUDE)/Parser.h


all:	$(BIN)/example1

$(BIN)/example1: $(SRC)/example1.cc $(INCLUDE)/Parser.h
	@echo " [CC] $@"
	@$(CPP) $(CPP_FLAGS) $< -o $@

clean:
	@echo " [Clean]"
	@$(RM) $(BIN)/example1
	@find $(SRC) -name "*.o" -exec rm {} \;


cleanvim:
	@echo " [Removing Vim temporary files (*~)]"
	@find . -name "*~" -exec rm {} \;
