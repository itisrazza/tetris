# Raresh's Tetris, Tetris Rarescha

This is a programming excerise for me to implement the classic game of Tetris
and make it feel nostalgic, while keeping (some) adherence to the modern
guidelines from The Tetris Company.

## Build

So far. I've only tested it on Linux. All you need is `make`, `g++` and the SDL2
library and friends in path.

After that: `make && ./tetris`

## Playing the game

### Keymap
| Key | Action |
|-----|-----|
| W | Hard drop |
| A | Move left |
| S | Soft drop |
| D | Move right |
| Q | Rotate counterclockwise |
| E | Rotate clockwise |
| F11 | Toggle full screen |
| ESC | Quit game |

## Wishlist

- [ ] Add a HUD to the game
- [ ] Add the ability to "hold" a piece
- [ ] Add a scoring system
- [ ] Add my own music synth (sampled audio is overrated)
  - [ ] Add chiptune [Korobeiniki]
  - [ ] Add sound effects
- [ ] Create a main menu + settings
- [ ] Multi-language support (Russian maybe?)

[Korobeiniki]: https://www.youtube.com/watch?v=umEDct4BoGc