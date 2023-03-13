CC = mpicc
CXX = mpicxx
CXXFLAGS = -std=c++17 -O3
CFLAGS = -O3
TARGETS = main

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	rm -f $(TARGETS)
