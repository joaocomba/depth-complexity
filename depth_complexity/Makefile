OS=$(shell uname -s)

GLFW_CFLAGS = $(shell pkg-config --cflags libglfw)
GLFW_LDFLAGS = $(shell pkg-config --libs libglfw)

ATB_CFLAGS = 
ATB_LDFLAGS = -lAntTweakBar

ifeq "$(OS)" "Darwin"
	GLUT_LDFLAGS += -framework GLUT -framework OpenGL
	GLEW_CFLAGS = -I/opt/local/include
	GLEW_LDFLAGS = -L/opt/local/lib -lGLEW
else
    ifeq "$(OS)" "Linux"
	    GLUT_LDFLAGS = -lglut -lGLU
    else
	    GLUT_LDFLAGS = -lglut
    endif
	GLEW_LDFLAGS = -lGLEW
endif

CPPFLAGS = $(GLFW_CFLAGS) $(ATB_CFLAGS) $(GLEW_CFLAGS) -Wall -Werror -O3
#CPPFLAGS = $(GLFW_CFLAGS) $(ATB_CFLAGS) $(GLEW_CFLAGS) -Wall -Werror -g
LDFLAGS = $(ATB_LDFLAGS) $(GLEW_LDFLAGS) -Wall -Werror
#LDFLAGS = $(ATB_LDFLAGS) $(GLEW_LDFLAGS) -Wall -Werror -g


.PHONY: all clean

all : main_joao depthcomplexity2d depthcomplexity3d_offline depthcomplexity3d

clean:
	rm -f *.o depthcomplexity3d depthcomplexity2d main_joao test_intersection depthcomplexity3d_offline

depthcomplexity3d: depthcomplexity3d.o camera.o dc_2d.o dc_3d.o util.o timer.o
	g++ -g -o $@ $^ $(LDFLAGS) $(GLFW_LDFLAGS) $(GLUT_LDFLAGS)

depthcomplexity2d: depthcomplexity2d.o dc_2d.o util.o flags.o timer.o
	g++ -g -o $@ $^ $(LDFLAGS) $(GLUT_LDFLAGS)

depthcomplexity3d_offline: \
	depthcomplexity3d_offline.o dc_3d.o dc_2d.o flags.o util.o timer.o
	g++ -g -o $@ $^ $(LDFLAGS) $(GLFW_LDFLAGS) $(GLUT_LDFLAGS)

main_joao: main_joao.o
	g++ -g -o $@ $^ $(LDFLAGS) $(GLUT_LDFLAGS)

