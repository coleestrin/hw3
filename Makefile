CXX = g++
CXXFLAGS = -Wall -std=c++11

all: cache_sim

cache_sim: cache_sim.cpp
	$(CXX) $(CXXFLAGS) -o cache_sim cache_sim.cpp

clean:
	rm -f cache_sim cache_sim_output

test: cache_sim
	@echo "1 3 5 1 3 1" > test_input
	@./cache_sim 4 2 test_input
	@echo "=== Output ==="
	@cat cache_sim_output
	@rm -f test_input