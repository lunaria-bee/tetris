GCC=g++

all: main.o tetris.o tetris_ui.o
	$(GCC) main.o tetris.o tetris_ui.o -lncursesw -o tetris

main.o: main.cpp
	$(GCC) main.cpp -c

tetris.o: tetris.cpp tetris.hpp
	$(GCC) tetris.cpp -c

tetris_ui.o: tetris_ui.cpp tetris_ui.hpp
	$(GCC) tetris_ui.cpp -c

clean:
	rm *.o tetris
