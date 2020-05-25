# 
# FF_ROOT     points to the FastFlow root directory (i.e.
#             the one containing the ff directory).
ifndef FF_ROOT 
FF_ROOT		= ${HOME}/fastflow
endif

CUR_DIRECTORY=$(pwd)
CXX		= g++ -std=c++17 
INCLUDES	= -I $(FF_ROOT) 
CXXFLAGS  	= -g # -DBLOCKING_MODE -DFF_BOUNDED_BUFFER

LDFLAGS 	= -pthread
OPTFLAGS	= -O3 -finline-functions -DNDEBUG

TARGETS		= TSP 


.PHONY: all clean cleanall
all: TSP

# library
build/agent.o: agent.cpp agent.h 
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(INCLUDES) -c agent.cpp -o build/agent.o 

build/tspAgent.o: tspAgent.cpp tspAgent.h agent.o
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(INCLUDES) -c tspAgent.cpp -o build/tspAgent.o 

build/population.o: population.cpp population.h agent.o tspAgent.o
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(INCLUDES) -c population.cpp -o build/population.o 
build/utils.o: utils.h utils.cpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(INCLUDES) -c utils.cpp  -o build/utils.o 

#all the main file
# %: %.cpp population.o
# 	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $< $(LDFLAGS)
build/TSP.o: TSP.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -c TSP.cpp -o build/TSP.o


TSP: build/TSP.o build/utils.o build/population.o build/agent.o build/tspAgent.o
	$(CXX) build/TSP.o build/utils.o build/population.o build/agent.o build/tspAgent.o -o TSP




clean		: 
	rm -f TSP
	rm -f build/*

cleanall	: clean
	\rm -f *.o *~
