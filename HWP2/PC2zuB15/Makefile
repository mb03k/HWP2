# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17
LDFLAGS = 

# Target executable
TARGET = main.elf

# Source files
SRCS = PC2_writeToB15.cpp writeToArduino.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files
HEADERS = writeToArduino.h

# Rule to build the target
$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

# Rule to compile .cpp files into .o files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
