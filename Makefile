# Compiler and flags
CXX = g++
CC = gcc
CXXFLAGS = -Wall -O2 -std=c++17 -Iinclude -I/usr/include/freetype2 -MMD -MP
CFLAGS = -Wall -O2 -Iinclude -I/usr/include/freetype2

# Directories
SRC_DIR = src
GLAD_DIR = src/glad
INCLUDE_DIR = include
BUILD_DIR = build

# Source files
MAIN_SRC = $(INCLUDE_DIR)/main.cpp $(SRC_DIR)/text_renderer.cpp $(SRC_DIR)/camera.cpp $(SRC_DIR)/input_handler.cpp $(SRC_DIR)/cube.cpp $(SRC_DIR)/player.cpp $(SRC_DIR)/player2.cpp $(SRC_DIR)/chessboard.cpp $(SRC_DIR)/draw_food_grid.cpp $(SRC_DIR)/agents.cpp $(SRC_DIR)/cube_grid.cpp
SERVER_SRC = $(SRC_DIR)/server.cpp
COMMON_SRC = $(GLAD_DIR)/glad.c

# Object files
MAIN_OBJ = $(addprefix $(BUILD_DIR)/, $(notdir $(MAIN_SRC:.cpp=.o))) $(BUILD_DIR)/$(notdir $(COMMON_SRC:.c=.o))
SERVER_OBJ = $(addprefix $(BUILD_DIR)/, $(notdir $(SERVER_SRC:.cpp=.o))) $(BUILD_DIR)/$(notdir $(COMMON_SRC:.c=.o))

# Targets
MAIN_TARGET = ChessboardWorld
SERVER_TARGET = Server

# Libraries
LDFLAGS = -lglfw -lGL -lfreetype -ldl -lX11 -lpthread -lm -lXrandr -lXi

# Default target
all: $(MAIN_TARGET) $(SERVER_TARGET)

# Build executables
$(MAIN_TARGET): $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(SERVER_TARGET): $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile C++ object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(INCLUDE_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile C object files (specifically for glad.c)
$(BUILD_DIR)/%.o: $(GLAD_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(BUILD_DIR) $(MAIN_TARGET) $(SERVER_TARGET)

# Debug build
debug: CXXFLAGS += -g
debug: clean all

# Include dependency files
-include $(MAIN_OBJ:.o=.d) $(SERVER_OBJ:.o=.d)

.PHONY: all clean debug
