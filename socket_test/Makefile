# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -O2

# Targets
TARGETS = server client

# Build all targets
all: $(TARGETS)

# Server target
server: server.cpp
	$(CXX) $(CXXFLAGS) -o server server.cpp

# Client target
client: client.cpp
	$(CXX) $(CXXFLAGS) -o client client.cpp

# Clean up generated files
clean:
	rm -f $(TARGETS)

# Phony targets
.PHONY: all clean
