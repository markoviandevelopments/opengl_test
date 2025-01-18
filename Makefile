# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra

# Libraries
LIBS = -lGL -lglfw

# Source files and output
SRCS = main.cpp src/glad/glad.c
OBJS = main.o src/glad/glad.o
TARGET = ChessboardWorld

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LIBS) -o $(TARGET)

# Compilation rules
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

src/glad/glad.o: src/glad/glad.c
	$(CXX) $(CXXFLAGS) -c src/glad/glad.c -o src/glad/glad.o

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Clean and rebuild
rebuild: clean all
