CXX=g++
CXXFLAGS=-std=gnu++0x -Wall -Werror

demux:	demux.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^
