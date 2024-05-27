
all: main

main: main.cpp
	g++ main.cpp -o main \
		`pkg-config --cflags --libs freetype2` \
		-lglfw -lGLU -lGL -lGLEW 

clean:
	rm -f main
