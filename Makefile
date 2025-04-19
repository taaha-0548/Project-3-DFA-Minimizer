# Compiler and flags
CXX         = g++
INCLUDE_DIR = include
CXXFLAGS    = -std=c++17 -Wall -I$(INCLUDE_DIR)

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Executable (with .exe extension)
TARGET = $(BIN_DIR)\dfa_minimization.exe

# Source and object files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Phony targets
.PHONY: all run clean

# Default target: compile and link
all: $(TARGET)

# Link step
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile step
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure output directories exist
$(OBJ_DIR):
	if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

$(BIN_DIR):
	if not exist $(BIN_DIR) mkdir $(BIN_DIR)

# Build & run
run: all
	$(TARGET)

# Clean up (Windows style)
clean:
	if exist $(OBJ_DIR) rmdir /S /Q $(OBJ_DIR)
	if exist $(BIN_DIR) rmdir /S /Q $(BIN_DIR)
