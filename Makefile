GCC=g++

all: main.o tetris.o tetris_ui.o
	g++ main.o tetris.o tetris_ui.o -lncursesw -o tetris

main.o: main.cpp
	g++ main.cpp -c

tetris.o: tetris.cpp tetris.hpp
	g++ tetris.cpp -c

tetris_ui.o: tetris_ui.cpp tetris_ui.hpp
	g++ tetris_ui.cpp -c

clean:
	rm *.o tetris
