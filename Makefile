CXX			:= g++
CXXFLAGS	:= -std=gnu++0x -Wall -Werror
ifeq ($(NDEBUG),)
  CXXFLAGS	+= -g -ggdb
endif

OBJS	:= demux.o
TARGET	:= demux

$(TARGET):	$(OBJS)
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
