# Credits:
# https://github.com/KRMisha/Makefile/blob/master/Makefile
# https://stackoverflow.com/questions/2394609/makefile-header-dependencies
# https://stackoverflow.com/questions/2214575/passing-arguments-to-make-run

CXX = g++
CXXFLAGS = -Wall -std=c++11 -pthread
LFLAGS = -lpthread
TARGET = prodcon

SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj
INCLUDE_DIR = include

SRCS = $(sort $(shell find $(SRC_DIR) -name '*.cpp'))
OBJECTS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES = $(OBJECTS:%.o=%.d)

.PHONY: all clean help run debug optimize optimize3

all: $(BIN_DIR)/$(TARGET)

debug: CXXFLAGS += -g
debug: all

optimize: CXXFLAGS += -O
optimize: all

optimize3: CXXFLAGS += -O3
optimize3: all

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)

help:
	@printf "\
	Usage: make [target] [options] \n\
	\n\
	Targets:\n\
	        all         Builds executable (default target), \n\
		run 	    Runs the exectuable with the passed arguments (look at options below) \n\
		clean       Deletes bin/ and obj/ directories storing the object files and executable \n\
		debug       Builds the executable with the -g flag (need to run make clean first!) \n\
		optimize    Builds the executable with the -O flag (need to run make clean first!) \n\
		optimize3   Builds the exectuable with the -O3 flag (need to run make clean first!) \n\
		help        Prints these instructions \n\
	\n\
	Options: \n\
	    nthreads <id> <path_to_input_file>         Possible options for the [run] target. \n\\n\
		nthreads = number of threads (mandatory)\n\
		<id> = id of log file (optional)\n\
		<path_to_input_file> = relative (to root directory) path to input file (optional) \n\\n"

# adopted from 
run: all
	@echo "Starting program: $(TARGET) $(filter-out $@,$(MAKECMDGOALS))"
	@cd ./$(BIN_DIR); ./$(TARGET) $(filter-out $@,$(MAKECMDGOALS))
%:
    @:

# This ensures make will not complain if these files don't exist *yet*
-include $(DEPENDENCIES)

# Build executable and link objects
$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@echo $(CXX) $(CXXFLAGS) $(LFLAGS) $^ -o $@
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) $(LFLAGS) $^ -o $@

# Compile source code to object files 
# the -MMD flag will ensure that dependencies for the source files i.e. headers will be included
# changes to the header files will trigger a re-compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo $(CXX) $(CXXFLAGS) -MMD -c $< -o $@
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@
