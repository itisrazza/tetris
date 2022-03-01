
#include "tetromino.h"

Block::Block()
{
    nil = true;
    red = 0; green = 0; blue = 0;
}

Tetromino::Tetromino()
{
    nil = true;
    size = 3;
}

Tetromino::tetromino_gen_t Tetromino::tetrominoes[] = {
    Tetromino::I, Tetromino::J, Tetromino::L,
    Tetromino::O, Tetromino::S, Tetromino::T, Tetromino::Z
};

Tetromino Tetromino::I() {
    Tetromino tetromino;
    tetromino.nil = false;

    Block block;
    block.nil = false;
    block.red = 0;
    block.green = 0xFF;
    block.blue = 0xFF;
    
    tetromino.size = 4;
    tetromino.blocks[1][0] = block;
    tetromino.blocks[1][1] = block;
    tetromino.blocks[1][2] = block;
    tetromino.blocks[1][3] = block;

    return tetromino;
}

Tetromino Tetromino::O() {
    Tetromino tetromino;
    tetromino.nil = false;

    Block block;
    block.nil = false;
    block.red = 0xFF;
    block.green = 0xFF;
    block.blue = 0;
    
    tetromino.size = 2;
    tetromino.blocks[0][0] = block;
    tetromino.blocks[0][1] = block;
    tetromino.blocks[1][0] = block;
    tetromino.blocks[1][1] = block;

    return tetromino;
}

Tetromino Tetromino::T() {
    Tetromino tetromino;
    tetromino.nil = false;

    Block block;
    block.nil = false;
    block.red = 0xFF;
    block.green = 0;
    block.blue = 0xFF;
    
    tetromino.size = 3;
    tetromino.blocks[0][1] = block;
    tetromino.blocks[1][0] = block;
    tetromino.blocks[1][1] = block;
    tetromino.blocks[1][2] = block;

    return tetromino;
}

Tetromino Tetromino::S() {  // tbd
    Tetromino tetromino;
    tetromino.nil = false;

    Block block;
    block.nil = false;
    block.red = 0;
    block.green = 0xFF;
    block.blue = 0;
    
    tetromino.size = 3;
    tetromino.blocks[0][1] = block;
    tetromino.blocks[0][2] = block;
    tetromino.blocks[1][0] = block;
    tetromino.blocks[1][1] = block;

    return tetromino;
}

Tetromino Tetromino::Z() {  // tbd
    Tetromino tetromino;
    tetromino.nil = false;

    Block block;
    block.nil = false;
    block.red = 0xFF;
    block.green = 0;
    block.blue = 0;
    
    tetromino.size = 3;
    tetromino.blocks[0][0] = block;
    tetromino.blocks[0][1] = block;
    tetromino.blocks[1][1] = block;
    tetromino.blocks[1][2] = block;

    return tetromino;
}

Tetromino Tetromino::J() {  // tbd
    Tetromino tetromino;
    tetromino.nil = false;

    Block block;
    block.nil = false;
    block.red = 0;
    block.green = 0;
    block.blue = 0xFF;
    
    tetromino.size = 3;
    tetromino.blocks[0][0] = block;
    tetromino.blocks[1][0] = block;
    tetromino.blocks[1][1] = block;
    tetromino.blocks[1][2] = block;

    return tetromino;
}

Tetromino Tetromino::L() {  // tbd
    Tetromino tetromino;
    tetromino.nil = false;

    Block block;
    block.nil = false;
    block.red = 0xFF;
    block.green = 0x88;
    block.blue = 0;
    
    tetromino.size = 3;
    tetromino.blocks[0][2] = block;
    tetromino.blocks[1][0] = block;
    tetromino.blocks[1][1] = block;
    tetromino.blocks[1][2] = block;

    return tetromino;
}


