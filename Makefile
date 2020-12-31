INCLUDE = `sdl2-config --cflags` `pkg-config --cflags glew glm SDL2_image SDL2_ttf`
LIBS = `sdl2-config --libs` `pkg-config --libs glew glm SDL2_image SDL2_ttf`
FRAMEWORK = -framework opengl
BINS = $(patsubst %.cpp, %.out, $(wildcard *.cpp))

all:${BINS}


${BINS}:%.out:%.cpp
	$(CXX) $^ -o $@ ${INCLUDE} ${LIBS} ${FRAMEWORK} -std=c++17

.PHONY:clean
clean:
	@echo ${BINS}
	-rm -rf *.out
	-rm -rf *.o
