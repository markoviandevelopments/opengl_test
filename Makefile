# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -O2 -std=c++17 -Iinclude -I/usr/include/freetype2
CC = gcc
CFLAGS = -Wall -O2 -Iinclude -I/usr/include/freetype2

# Source files for each target
MAIN_SRC = include/main.cpp src/text_renderer.cpp src/camera.cpp src/input_handler.cpp src/cube.cpp src/player.cpp src/player2.cpp
SERVER_SRC = src/server.cpp
COMMON_SRC = src/glad/glad.c

# Object files
MAIN_OBJ = $(MAIN_SRC:.cpp=.o) $(COMMON_SRC:.c=.o)
SERVER_OBJ = $(SERVER_SRC:.cpp=.o) $(COMMON_SRC:.c=.o)

# Target executables
MAIN_TARGET = ChessboardWorld
SERVER_TARGET = Server

# Libraries
LDFLAGS = -lglfw -lGL -lfreetype -ldl -lX11 -lpthread -lm -lXrandr -lXi

# Default target: Build both ChessboardWorld and Server
all: $(MAIN_TARGET) $(SERVER_TARGET)

# Build ChessboardWorld
$(MAIN_TARGET): $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Build Server
$(SERVER_TARGET): $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile C++ object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile C object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(MAIN_OBJ) $(SERVER_OBJ) $(MAIN_TARGET) $(SERVER_TARGET)

.PHONY: all clean
