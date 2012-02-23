CC			:=	gcc
CXX			:=	g++
CXXFLAGS	:=	$(shell sdl-config --cflags)
LDFLAGS		:=	$(shell sdl-config --libs)

TGT			:=	sam

OBJS		:= $(addprefix obj/, main.o utils.o ReplayCodecManager.o ReplayInterfaces.o ReplayDemuxer.o ReplayCatalogue.o)

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

obj/%.o obj/%.d: %.c
ifeq ($(QUIET),@)
	@echo "Compiling $<"
endif
	$(QUIET)$(CC) -MMD -MP -MT $@ -c $(CFLAGS) $< -o $@

obj/%.o obj/%.d: %.cpp
ifeq ($(QUIET),@)
	@echo "Compiling $<"
endif
	$(QUIET)$(CXX) -MMD -MP -MT $@ -c $(CXXFLAGS) $< -o $@

-include $(OBJS:.o=.d)

