# Dungeon Goofs

A simple C++ terminal game inspired by classic 'Hello World' projects, designed to explore ncurses (PDCurses) functionality and basic game logic.

<img width="959" height="512" alt="image" src="https://github.com/user-attachments/assets/1360b2be-af66-49b7-93ee-b1bdfdb1e5a0" />


## Overview

Dungeon Goofs is a minimal, game-like application that generates a random dungeon map filled with obstacles and food. Autonomous units called "goofs" move around the map, consuming food and leaving trails. When a goof eats food, it spawns a "pseudogoof" (a sterile copy). The game ends when there is no free space left on the map.

## Features
- Randomly generated dungeon map with obstacles and food
- Autonomous movement of goofs
- Food consumption and pseudogoof spawning
- Simple terminal-based UI using ncurses (PDCurses)
- Game ends when the map is full

## Gameplay
1. **Start the game:**
   - Run the program in a terminal that supports ncurses (e.g., Windows terminal with PDCurses).
   - The main menu appears. Select `1) New game` and press the `j` key to start.
2. **Game mechanics:**
   - The game area is generated with random obstacles (`X`) and food (`f`).
   - Goofs (`@`) are placed randomly and move around the map.
   - When a goof eats food, it leaves a `$` and spawns a pseudogoof at its previous position.
   - Goofs avoid obstacles, walls, and each other.
   - The game displays the remaining free space and the current unit count at the bottom.
   - Press `q` at any time to quit the game.

## Controls
- **Main menu:**
  - `1` — New game (press `j` to confirm and start)
  - `2` — Options (not implemented)
  - `3` — Quit
- **In-game:**
  - `q` — Quit the game

## Requirements
- C++ compiler
- PDCurses or ncurses library (for Windows, PDCurses is included in the `lib/` directory)

## Building
1. Make sure you have a C++ compiler and PDCurses (or ncurses) installed.
2. Compile the source file:
   ```sh
   g++ -Iinclude -Llib -lpdcurses src/dungeon_goofs.cpp -o bin/dungeon_goofs.exe
   ```
   Adjust the command as needed for your environment and library locations.

## Project Structure
- `src/dungeon_goofs.cpp` — Main source code
- `include/` — Header files (curses.h, etc.)
- `lib/` — PDCurses libraries
- `bin/` — Compiled executable
- `res/` — Resources (icons, etc.)

## License
This project is for educational purposes. 
