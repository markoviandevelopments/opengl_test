# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -O2 -std=c++17 -Iinclude -I/usr/include/freetype2

# Source and object files
SRC = include/main.cpp src/text_renderer.cpp src/glad/glad.c
OBJ = $(SRC:.cpp=.o)

# Target executable
TARGET = ChessboardWorld

# Libraries
LDFLAGS = -lglfw -lGL -lfreetype -ldl -lX11 -lpthread -lm -lXrandr -lXi

# Build the executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: clean
