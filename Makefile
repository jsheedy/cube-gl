cube-gl: cube-gl.cpp shaders/wood-cube.vs shaders/wood-cube.fs shaders/lamp.vs shaders/lamp.fs
	g++ -g `pkg-config --cflags glfw3` -o cube-gl cube-gl.cpp glad.c  `pkg-config --static --libs glfw3` -I .

clean:
	rm cube-gl