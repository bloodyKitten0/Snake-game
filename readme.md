

# Snake Game

A terminal-based Snake game written in **C++20**, featuring multiple food types, dynamic difficulty, collision detection, and a separately testable game-logic layer.

## Features

- **Grid-based movement:** A 20 × 40 game board.
- **Multiple normal food items:** Up to 15 normal food items can exist simultaneously.
- **Big food:** A 4% spawn chance per eligible spawn attempt. Big food occupies a 2 × 2 area and increases the snake's length by four.
- **Collision detection:** Wall collisions and self-collisions.
- **Growth system:** The snake grows after consuming food.
- **Dynamic difficulty:** Movement speed increases as the score rises.
- **Direction validation:** Prevents the snake from reversing directly into itself.
- **Automated tests:** Tests for movement, collisions, food consumption, placement, and direction changes.

## Technologies

- C++20
- CMake
- GNU C++ Compiler (G++)
- C++ Standard Library
- POSIX terminal interfaces (`termios`, `unistd`)

## Project Structure

```text
snake/
├── .github/
│   └── workflows/
│       └── tests.yml
├── images/
├── .gitignore
├── CMakeLists.txt
├── LICENSE
├── game_logic.hpp
├── main.cpp
├── readme.md
└── tests.cpp
```

### Architecture

| File | Responsibility |
|---|---|
| `game_logic.hpp` | Game state, movement, collision detection, food generation, and game rules. |
| `main.cpp` | Terminal input, rendering, game loop, and user interaction. |
| `tests.cpp` | Automated tests for the game logic. |
| `CMakeLists.txt` | Build configuration and test registration. |
| `.github/workflows/tests.yml` | Automated build and test workflow. |

The game logic is separated from the terminal interface, allowing its core behaviour to be tested independently.

## Requirements

- A C++20-compatible compiler.
- CMake 3.20 or newer.
- A POSIX-compatible environment (Linux or macOS).

## Building

Clone the repository:

```bash
git clone https://github.com/bloodyKitten0/Snake-game.git
cd Snake-game
```

Configure the project:

```bash
cmake -S . -B build
```

Build the game and tests:

```bash
cmake --build build
```

## Running

```bash
./build/snake
```

## Controls

| Key | Action |
|---|---|
| `W` | Move up |
| `A` | Move left |
| `S` | Move down |
| `D` | Move right |
| `Q` | Quit |

## Testing

Run the automated test suite:

```bash
ctest --test-dir build --output-on-failure
```

The tests cover:

- Normal movement.
- Wall collisions.
- Self-collisions.
- Movement into a vacated tail position.
- Normal food consumption.
- Big food consumption.
- Big food placement validation.
- Food generation.
- Normal food limits.
- Direction reversal prevention.

## Continuous Integration

GitHub Actions automatically builds and tests the project when changes are pushed or pull requests are opened.

## Purpose

This project was developed to practise C++ programming, data structures, state management, modular code organisation, automated testing, and build-system configuration.

It serves as a foundational systems-programming exercise before progressing to more advanced C++ and machine-learning infrastructure projects.

