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

.PHONY: all clean

all: $(BIN_DIR)/$(CLIENT) $(BIN_DIR)/$(SERVER)
	# TODO: REMOVE THIS
	$(shell chmod +x bin/client bin/server)

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)
	rm client.pdf
	rm server.pdf

# This ensures make will not complain if these files don't exist *yet*
-include $(CLIENT_DEPENDENCIES) $(SERVER_DEPENDENCIES) $(COMMON_DEPENDENCIES)

$(BIN_DIR)/$(CLIENT): $(CLIENT_OBJECTS) $(COMMON_OBJECTS) 
	@echo $(CXX) $(CXXFLAGS) $^ -o $@
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) $^ -o $@
	@groff -Tpdf -man client.1 > client.pdf

$(BIN_DIR)/$(SERVER): $(SERVER_OBJECTS) $(COMMON_OBJECTS) 
	@echo $(CXX) $(CXXFLAGS) $^ -o $@
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) $^ -o $@
	@groff -Tpdf -man client.1 > server.pdf

# Compile source code to object files 
# the -MMD flag will ensure that dependencies for the source files i.e. headers will be included
# changes to the header files will trigger a re-compile
$(OBJ_DIR)/$(CLIENT)/%.o: $(SRC_DIR)/$(CLIENT)/%.cpp 
	@echo $(CXX) $(CXXFLAGS) -MMD -c $< -o $@
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

$(OBJ_DIR)/$(SERVER)/%.o: $(SRC_DIR)/$(SERVER)/%.cpp 
	@echo $(CXX) $(CXXFLAGS) -MMD -c $< -o $@
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo $(CXX) $(CXXFLAGS) -MMD -c $< -o $@
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@
	