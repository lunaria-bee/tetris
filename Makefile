CC=g++

all: main.o tetris.o tetris_ui.o
	$(CC) main.o tetris.o tetris_ui.o -lncursesw -o tetris

main.o: main.cpp
	$(CC) main.cpp -c

tetris.o: tetris.cpp tetris.hpp
	$(CC) tetris.cpp -c

tetris_ui.o: tetris_ui.cpp tetris_ui.hpp
	$(CC) tetris_ui.cpp -c

clean:
	rm *.o tetris
