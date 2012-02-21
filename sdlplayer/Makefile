CC			:=	gcc
CXX			:=	g++
CXXFLAGS	:=	$(shell sdl-config --cflags)
LDFLAGS		:=	$(shell sdl-config --libs)

sam:	main.o utils.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)
