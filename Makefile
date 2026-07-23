# ==============================================================================
# Makefile for GTK+3 C++ Escape Room Project
# ==============================================================================

# Compiler & Flags
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Query GTK+3 compile and linker flags automatically using pkg-config
GTK_CFLAGS := $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS   := $(shell pkg-config --libs gtk+-3.0)

# Target Executable Name
TARGET := EscapeRoomApp

# Source and Object Files
SRCS := main.cpp EscapeRoom.cpp
OBJS := $(SRCS:.cpp=.o)

# ------------------------------------------------------------------------------
# Build Rules
# ------------------------------------------------------------------------------

# Default target
all: $(TARGET)

# Link object files to create the final executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(GTK_LIBS)
	@echo "Build successful! Run './$(TARGET)' to start the game."

# Compile C++ source files into object files
%.o: %.cpp EscapeRoom.h
	$(CXX) $(CXXFLAGS) $(GTK_CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Cleaned build files."

# Run the program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
