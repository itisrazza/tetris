
CXX = g++
CXX_OPTS = -Wall -Wextra -g -I/usr/include/SDL2
LD_OPTS = -lSDL2

headers = tetromino.h

.PHONY: clean

tetris: main.o tetris.o tetromino.o
	$(CXX) $(CXX_OPTS) -o $@ $^ $(LD_OPTS)

%.o: %.cpp $(headers)
	$(CXX) $(CXX_OPTS) -c -o $@ $<

clean:
	rm -f tetris tetris.exe *.bin *.o
