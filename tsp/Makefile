# ---------------------------------------------------------------------------
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as 
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#  As a special exception, you may use this file as part of a free software
#  library without restriction.  Specifically, if other files instantiate
#  templates or use macros or inline functions from this file, or you compile
#  this file and link it with other files to produce an executable, this
#  file does not by itself cause the resulting executable to be covered by
#  the GNU General Public License.  This exception does not however
#  invalidate any other reasons why the executable file might be covered by
#  the GNU General Public License.
#
# ---------------------------------------------------------------------------

# 
# FF_ROOT     points to the FastFlow root directory (i.e.
#             the one containing the ff directory).
#
# PGA_ROOT points to the parallel genetic algorithm root direcotory

ifndef FF_ROOT 
FF_ROOT		= ${HOME}/fastflow
endif

ifndef PGA_ROOT 
PGA_ROOT	= ${HOME}/PGA
endif

CXX		= g++ -std=c++17 
INCLUDES	= -I $(FF_ROOT) -I $(PGA_ROOT)
CXXFLAGS  	= -g -DBLOCKING_MODE -DFF_BOUNDED_BUFFER

LDFLAGS 	= -pthread -fopenmp
OPTFLAGS	= -O3 -finline-functions -DNDEBUG

TARGETS		= TSP \
		  	TSP_ff \
			  TSP_thread \
			  TSP_parfor

.PHONY: all clean cleanall
.SUFFIXES: .cpp 


%: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o build/$@ $< $(LDFLAGS)

all		: $(TARGETS)

clean		: 
	rm -f build/*

cleanall	: clean
	\rm -f *.o *~
