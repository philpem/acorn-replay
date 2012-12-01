CC			:=	gcc
CXX			:=	g++
CXXFLAGS	:=	$(shell sdl-config --cflags) -g -ggdb
LDFLAGS		:=	$(shell sdl-config --libs)

TGT			:=	sam

OBJS		:= $(addprefix obj/, main.o utils.o ReplayCodecManager.o ReplayInterfaces.o ReplayDemuxer.o ReplayCatalogue.o ReplayDemuxer.o)

.PHONY: all tidy clean

all:	$(TGT)

tidy:
	-rm -f *~

clean:
	-rm -f *~
	-rm -f $(OBJS) $(TGT)

$(TGT):	$(OBJS)
	@echo "Building executable $@"
	$(QUIET)$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

obj/%.o: %.c
ifeq ($(QUIET),@)
	@echo "Compiling $<"
endif
	$(QUIET)$(CC) -MMD -MP -MT $@ -c $(CFLAGS) $< -o $@

obj/%.o: %.cpp
ifeq ($(QUIET),@)
	@echo "Compiling $<"
endif
	$(QUIET)$(CXX) -MMD -MP -MT $@ -c $(CXXFLAGS) $< -o $@

-include $(OBJS:.o=.d)

