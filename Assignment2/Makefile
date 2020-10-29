# Adapted from https://github.com/KRMisha/Makefile/blob/master/Makefile
CXX = g++
# TODO: REMOVE -g
CXXFLAGS = -Wall -O -g -std=c++11 -pthread
LFLAGS = -lpthread
TARGET = prodcon

SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj
INCLUDE_DIR = include

SRCS = $(sort $(shell find $(SRC_DIR) -name '*.cpp'))
OBJECTS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS = constants.hpp

.PHONY: all clean run

all: $(BIN_DIR)/$(TARGET)

clean:
	@echo "Cleaning prodcon project: removing $(BIN_DIR) and $(OBJ_DIR) directories" 
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)

# adopted from https://stackoverflow.com/questions/2214575/passing-arguments-to-make-run
run: all
	@echo "Starting program: $(TARGET) $(filter-out $@,$(MAKECMDGOALS))"
	rm ./output/prodcon.1.log
	@cd ./$(BIN_DIR); ./$(TARGET) $(filter-out $@,$(MAKECMDGOALS))
%:
    @:

# Build executable and link objects
$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@echo "Building executable: $@"
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) $(LFLAGS) $^ -o $@

# Compile source code to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDE_DIR)/$(DEPS)
	@echo "Compiling: $<"
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Include header dependencies for source files
$(SRC_DIR)/%.cpp: $(INCLUDE_DIR)/%.hpp 