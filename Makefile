# 
# FF_ROOT     points to the FastFlow root directory (i.e.
#             the one containing the ff directory).
ifndef FF_ROOT 
FF_ROOT		= ${HOME}/fastflow
endif

CUR_DIRECTORY=$(pwd)
CXX		= g++ -std=c++17 

CXXFLAGS  	= -g -DBLOCKING_MODE -DFF_BOUNDED_BUFFER
INCLUDES	= -I $(FF_ROOT) 

LDFLAGS 	= -pthread
OPTFLAGS	= -O3 -finline-functions -DNDEBUG

TARGETS		= TSP \
				TSP_ff


.PHONY: all clean cleanall
.SUFFIXES: .cpp 


%: %.cpp population.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $< $(LDFLAGS)

all             : $(TARGETS)

clean           : 
	rm -f $(TARGETS)

cleanall        : clean
	\rm -f *.o *~

