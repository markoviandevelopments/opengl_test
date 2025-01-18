# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -O2 -std=c++17 -Iinclude -I/usr/include/freetype2
CC = gcc
CFLAGS = -Wall -O2 -Iinclude -I/usr/include/freetype2

# Source and object files
CPP_SRC = include/main.cpp src/text_renderer.cpp src/camera.cpp
C_SRC = src/glad/glad.c
OBJ = $(CPP_SRC:.cpp=.o) $(C_SRC:.c=.o)

# Target executable
TARGET = ChessboardWorld

# Libraries
LDFLAGS = -lglfw -lGL -lfreetype -ldl -lX11 -lpthread -lm -lXrandr -lXi

# Build the executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile C++ object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile C object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: clean
