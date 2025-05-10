# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -O2 -std=c++17

# Target executable
TARGET = bitonic_serial

# Source file
SRC = bitonic_serial.cpp

all: $(TARGET) pth_cuda
# Build target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

pth_cuda: bitonic_pthread.c bitonic_cuda.cu
	gcc -std=c++11 -pthread bitonic_pthread.c -o bitonic_pthread
	nvcc -o bitonic_cuda bitonic_cuda.cu -std=c++11

run_pthread: pth_cuda
#  <element 2^x> <thread 2^y>
	./bitonic_pthread 10 1 

run_cuda: pth_cuda
#  <element 2^x>
	./bitonic_cuda 10

clean:
	rm -f $(TARGET) bitonic_pthread bitonic_cuda
