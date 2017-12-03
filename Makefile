all: cube-geometry-shader

cube-gl: cube-gl.cpp shaders/wood-cube.vs shaders/wood-cube.fs shaders/lamp.vs shaders/lamp.fs
	g++ -g `pkg-config --cflags glfw3` -o cube-gl cube-gl.cpp glad.c  `pkg-config --static --libs glfw3` -I .

cube-gl2: cube-gl2.cpp shaders/brain-glow.vs shaders/brain-glow.fs shaders/lamp.vs shaders/lamp.fs
	g++ -g `pkg-config --cflags glfw3` -o cube-gl2 cube-gl2.cpp glad.c  `pkg-config --static --libs glfw3` -I .

cube-geometry-shader: cube-geometry-shader.cpp shader.h shaders/brain-glow.vs shaders/brain-glow.fs shaders/geometry.vs shaders/geometry.fs shaders/geometry.gs
	g++ -std=c++11 `pkg-config --cflags liblo` `pkg-config --libs liblo` `pkg-config --cflags glfw3` -o cube-geometry-shader cube-geometry-shader.cpp glad.c  `pkg-config --static --libs glfw3` -I .

clean:
	rm cube-gl? cube-geometry-shader