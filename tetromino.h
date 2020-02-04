#pragma once

#include <stddef.h>

struct Block {
    bool nil;

    unsigned char red;
    unsigned char green;
    unsigned char blue;

    Block();
};

struct Tetromino {
    bool nil;
    int size;
    
    Block blocks[4][4];

    Tetromino();
    
    typedef Tetromino (*tetromino_gen_t)();

    static tetromino_gen_t tetrominoes[];
    static const size_t tetrominoes_count = 7;

    static Tetromino I();
    static Tetromino O();
    static Tetromino T();
    static Tetromino S();
    static Tetromino Z();
    static Tetromino J();
    static Tetromino L();
};
