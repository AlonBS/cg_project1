INCFLAGS  = -I /usr/include/GL -I ./utilities/include
#INCFLAGS += -I ~/programming/Computer_Graphics_Exs/Computer_Graphics/Ex0_MIT/mit-vecmath

#INCFLAGS += -I /mit/6.837/public/include/vecmath

LINKFLAGS  = -lGL -lGLU -lassimp  -lSOIL -lGLEW -lglfw -L ./utilities/libs -lfreeimage
#LINKFLAGS += -L /mit/6.837/public/lib -lvecmath
#LINKFLAGS += -L ~/programming/Computer_Graphics_Exs/Computer_Graphics/Ex0_MIT/mit-vecmath/output -lvecmath


CFLAGS    = -g -std=c++1y
CC        = g++
SRCS      = main.cpp utilities/program.cpp utilities/camera.cpp utilities/mesh.cpp utilities/model.cpp
OBJS      = $(SRCS:.cpp=.o)
PROG      = project1

all: $(SRCS) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(INCFLAGS) $(LINKFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -c -o $@ $(INCFLAGS)

depend:
	makedepend $(INCFLAGS) -Y $(SRCS)

clean:
	rm $(OBJS) $(PROG)

