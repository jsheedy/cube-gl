CPP_FLAGS=-std=c++11
# CPP_FLAGS=-std=c++11 -g

all: procedural-terrain

cube-gl: cube-gl.cpp shaders/vertex/wood-cube.vs shaders/fragment/wood-cube.fs shaders/vertex/lamp.vs shaders/fragment/lamp.fs
	g++ -g `pkg-config --cflags glfw3` -o cube-gl cube-gl.cpp glad.c  `pkg-config --static --libs glfw3` -I .

cube-gl2: cube-gl2.cpp
	g++ -g `pkg-config --cflags glfw3` -o cube-gl2 cube-gl2.cpp glad.c  `pkg-config --static --libs glfw3` -I .

cube-geometry-shader: objects.hpp scene.hpp osc.hpp osc.cpp cube-geometry-shader.cpp shader.h shaders/brain-glow.vs
	g++ $(CPP_FLAGS) `pkg-config --cflags liblo` `pkg-config --libs liblo` `pkg-config --cflags glfw3` -o cube-geometry-shader cube-geometry-shader.cpp osc.cpp glad.c  `pkg-config --static --libs glfw3` -I .

model-scene: model-scene.cpp model.hpp objects.hpp scene.hpp osc.hpp osc.cpp shader.h shaders/vertex/brain-glow.vs shaders/fragment/brain-glow.fs shaders/vertex/geometry.vs shaders/fragment/geometry.fs shaders/geometry/geometry.gs
	g++ $(CPP_FLAGS) `pkg-config --libs assimp` `pkg-config --cflags liblo` `pkg-config --libs liblo` `pkg-config --cflags glfw3` -o model-scene model-scene.cpp osc.cpp glad.c  `pkg-config --static --libs glfw3` -I .

procedural-terrain: *.cpp *.h *.hpp shaders/*/*
	g++ $(CPP_FLAGS) `pkg-config --libs assimp` `pkg-config --cflags liblo` `pkg-config --libs liblo` `pkg-config --cflags glfw3` -o procedural-terrain procedural-terrain.cpp osc.cpp glad.c  `pkg-config --static --libs glfw3` -I .

clean:
	rm cube-gl? cube-geometry-shader model-scene procedural-terrain
