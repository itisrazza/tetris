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
| H | Hold tetromino |
| F11 | Toggle full screen |
| ESC | Quit game |

## Acknowledgements

### Original Work

The original concept was made by Alexei Pajitnov at the Soviet Academy of Sciences in Moscow and has since exploded in popularity.

### Font

`font.ttf` is from the Ultimate Old-School PC Font Pack made by VileR and is licensed under [Creative License Attribution-ShareAlike 4.0 International License](https://creativecommons.org/licenses/by-sa/4.0/).

You can get the original font and a lot more from: https://int10h.org/oldschool-pc-fonts/

## Wishlist

- [ ] Add a HUD to the game
- [x] Add the ability to "hold" a piece
- [ ] Add a scoring system
- [ ] Add my own music synth (sampled audio is overrated)
  - [ ] Add chiptune [Korobeiniki]
  - [ ] Add sound effects
- [ ] Create a main menu + settings
- [ ] Multi-language support (Russian maybe?)

[Korobeiniki]: https://www.youtube.com/watch?v=umEDct4BoGc