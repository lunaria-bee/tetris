CXX=g++

all: tetris

tetris: main.o tetris.o tetris_ui.o
	$(CXX) $^ -lncursesw -o tetris

main.o: main.cpp
	$(CXX) main.cpp -c

%.o: %.cpp %.hpp
	$(CXX) $< -c

clean:
	rm *.o tetris
