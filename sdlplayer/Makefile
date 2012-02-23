CC			:=	gcc
CXX			:=	g++
CXXFLAGS	:=	$(shell sdl-config --cflags)
LDFLAGS		:=	$(shell sdl-config --libs)

TGT			:=	sam
OBJS		:=	main.o utils.o ReplayCodecManager.o ReplayInterfaces.o ReplayDemuxer.o ReplayCatalogue.o

.PHONY: all tidy clean

all:	$(TGT)

tidy:
	-rm -f *~

clean:
	-rm -f *~
	-rm -f $(OBJS) $(TGT)

$(TGT):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
