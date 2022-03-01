
CXX = g++
CXX_OPTS = -Wall -Wextra -std=c++14 -g -I/usr/include/SDL2
LD_OPTS = -lSDL2 -lSDL2_ttf

objects = system.o data.o mode.o tetris.o tetromino.o title.o
headers = mode.h title.h tetris.h tetromino.h

.PHONY: default clean

default: bin bin/tetris.exe

bin/tetris.exe: $(objects:%.o=obj/%.o)
	$(CXX) $(CXX_OPTS) -o $@ $^ $(LD_OPTS)

obj/data.cpp: $(wildcard data/*)
	python3 scripts/bin2cpp.py $@ $^

obj/%.o: src/%.cpp $(headers:%.h=src/%.h) obj
	$(CXX) $(CXX_OPTS) -c -o $@ $<

bin obj:
	mkdir -p $@

clean:
	rm -rf bin obj
