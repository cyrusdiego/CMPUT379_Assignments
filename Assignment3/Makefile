# Credits:
# Adapted from: https://github.com/KRMisha/Makefile/blob/master/Makefile
# Auto-generate dependencies: https://stackoverflow.com/questions/2394609/makefile-header-dependencies

CXX = g++
CXXFLAGS = -Wall -std=c++11 -O
CLIENT = client
SERVER = server

SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj
INCLUDE_DIR = include

CLIENT_DIR = $(SRC_DIR)/$(CLIENT)
SERVER_DIR = $(SRC_DIR)/$(SERVER)
CLIENT_OBJ_DIR = $(OBJ_DIR)/$(CLIENT)
SERVER_OBJ_DIR = $(OBJ_DIR)/$(SERVER)

CLIENT_SRCS := $(sort $(shell find $(CLIENT_DIR) -name '*.cpp'))
SERVER_SRCS := $(sort $(shell find $(SERVER_DIR) -name '*.cpp'))
COMMON_SRCS = $(wildcard $(SRC_DIR)/*.cpp)

CLIENT_OBJECTS = $(CLIENT_SRCS:$(CLIENT_DIR)/%.cpp=$(OBJ_DIR)/$(CLIENT)/%.o)
SERVER_OBJECTS = $(SERVER_SRCS:$(SERVER_DIR)/%.cpp=$(OBJ_DIR)/$(SERVER)/%.o)
COMMON_OBJECTS = $(COMMON_SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CLIENT_DEPENDENCIES = $(CLIENT_OBJECTS:%.o=%.d)
SERVER_DEPENDENCIES = $(SERVER_OBJECTS:%.o=%.d)
COMMON_DEPENDENCIES = $(COMMON_OBJECTS:%.o=%.d)

.PHONY: all clean help run 

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

# credits to Johan Dahlin and Idelic 
# https://stackoverflow.com/questions/2214575/passing-arguments-to-make-run
# If the first argument is "run"...
# ifeq (run,$(firstword $(MAKECMDGOALS)))
#   # use the rest as arguments for "run"
#   RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
#   # ...and turn them into do-nothing targets
#   $(eval $(RUN_ARGS):;@:)
# endif

# run: all
# 	@echo "Starting program: $(TARGET) $(RUN_ARGS)"
# 	./$(BIN_DIR)/$(TARGET) $(RUN_ARGS)

# %:
#     @:

all: $(BIN_DIR)/$(CLIENT) $(BIN_DIR)/$(SERVER)
	# TODO: REMOVE THIS
	$(shell chmod +x bin/client bin/server)

# This ensures make will not complain if these files don't exist *yet*
-include $(CLIENT_DEPENDENCIES) $(SERVER_DEPENDENCIES) $(COMMON_DEPENDENCIES)

$(BIN_DIR)/$(CLIENT): $(CLIENT_OBJECTS) $(COMMON_OBJECTS)
	@echo $(CXX) $(CXXFLAGS) $^ -o $@
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) $^ -o $@

# $(BIN_DIR)/$(SERVER): $(SERVER_OBJECTS) $(COMMON_OBJ)
# 	@echo $(CXX) $(CXXFLAGS) $^ -o $@
# 	@mkdir -p $(@D)
# 	@$(CXX) $(CXXFLAGS) $^ -o $@

# Compile source code to object files 
# the -MMD flag will ensure that dependencies for the source files i.e. headers will be included
# changes to the header files will trigger a re-compile
$(OBJ_DIR)/$(CLIENT)/%.o: $(SRC_DIR)/$(CLIENT)/%.cpp 
	@echo $(CXX) $(CXXFLAGS) -MMD -c $< -o $@
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# $(SERVER_OBJECTS): $(SERVER_SRCS)
# 	@echo $(CXX) $(CXXFLAGS) -MMD -c $< -o $@
# 	@mkdir -p $(@D)
# 	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo $(CXX) $(CXXFLAGS) -MMD -c $< -o $@
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@
