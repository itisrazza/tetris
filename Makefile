
CXX = g++
CXX_OPTS = -Wall -Wextra -g -I/usr/include/SDL2 
LD_OPTS = -lSDL2 -lSDL2_ttf

headers = mode.h title.h tetris.h tetromino.h

.PHONY: clean

tetris: system.o mode.o tetris.o tetromino.o title.o
	$(CXX) $(CXX_OPTS) -o $@ $^ $(LD_OPTS)

%.o: %.cpp $(headers)
	$(CXX) $(CXX_OPTS) -c -o $@ $<
clean:
	rm -f tetris tetris.exe *.bin *.o
