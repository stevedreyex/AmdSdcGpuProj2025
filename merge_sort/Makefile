# Compiler and flags
CXX = g++
NVCC = nvcc -std=c++11
CXXFLAGS = -o 
NVCCFLAGS = -o
ELEMENT_NUM = 65536
THREAD_NUM = 4

# Targets
all: run

# Compile and generate executables
rand: rand.cpp
	$(CXX) $(CXXFLAGS) rand rand.cpp

merge-serial: merge-serial.cpp
	$(CXX) $(CXXFLAGS) merge-serial merge-serial.cpp

merge-pthread: merge-pthread.cpp
	$(CXX) $(CXXFLAGS) merge-pthread merge-pthread.cpp

merge-cuda: merge-cuda.cu
	$(NVCC) $(NVCCFLAGS) merge-cuda merge-cuda.cu

# Run the full pipeline
run: rand merge-serial merge-pthread merge-cuda
	./rand $(ELEMENT_NUM) > dataset.txt
	./merge-serial $(ELEMENT_NUM) < dataset.txt > output.txt
	./merge-pthread $(ELEMENT_NUM) $(THREAD_NUM) < dataset.txt >> output.txt
	./merge-cuda $(ELEMENT_NUM) < dataset.txt >> output.txt

# Clean up generated files
clean:
	rm -f rand merge-serial merge-pthread merge-cuda dataset.txt output.txt

analysis: rand merge-serial merge-cuda
	./run_analysis.sh
	