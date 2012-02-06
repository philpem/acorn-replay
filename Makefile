CXX=g++
CXXFLAGS=-std=gnu++0x

demux:	demux.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^
