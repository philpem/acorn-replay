CXX			:= g++
CXXFLAGS	:= -std=gnu++0x -Wall
ifeq ($(NDEBUG),)
  CXXFLAGS	+= -g -ggdb
endif

.PHONY: all clean

###############

all:	demux mulaw ycbcr

clean:
	-rm -f $(OBJS) $(TARGET)

###############

demux: demux.o
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^

mulaw: mulaw.o
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^

ycbcr: ycbcr.o
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(LDFLAGS) -lpthread -lX11 -o $@ $^

