# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra
LDFLAGS = -lcurl -pthread

# Source and object files
SRC = ollama_cpp.cpp
OBJ = $(SRC:.cpp=.o)
EXEC = ollama_cpp

# Build target
all: $(EXEC)

$(EXEC): $(OBJ)
    $(CXX) $(CXXFLAGS) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Clean build artifacts
clean:
    rm -f $(OBJ) $(EXEC)
