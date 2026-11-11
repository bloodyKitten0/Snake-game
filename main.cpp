#include "game_logic.hpp"
#include <algorithm>
#include <chrono>
#include <csignal>
#include <cstring>
#include <iostream>
#include <random>
#include <termios.h>
#include <thread>
#include <unistd.h>

using namespace Snake;

volatile std::sig_atomic_t stopRequested = 0;

void handleSignal(int) {
    stopRequested = 1;
}

class Terminal {
private:
    termios original{};
    bool configured = false;

public:
    Terminal() {
        if (tcgetattr(STDIN_FILENO, &original) == -1) {
            throw std::runtime_error("Could not read terminal settings");
        }

        termios raw = original;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;

        if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) {
            throw std::runtime_error("Could not configure terminal");
        }

        configured = true;
    }

    ~Terminal() {
        if (configured) {
            tcsetattr(STDIN_FILENO, TCSANOW, &original);
            std::cout << "\033[?25h" << std::flush;
        }
    }

    Terminal(const Terminal&) = delete;
    Terminal& operator=(const Terminal&) = delete;
};

char readInput() {
    char input;
    ssize_t result = read(STDIN_FILENO, &input, 1);

    if (result == 1) {
        return input;
    }

    return '\0';
}

void render(const GameState& game) {
    std::cout << "\033[H";

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            Position current{x, y};

            if (
                x == 0 || x == WIDTH - 1 ||
                y == 0 || y == HEIGHT - 1
            ) {
                std::cout << '#';
            }
            else if (game.snake.front() == current) {
                std::cout << 'O';
            }
            else if (
                contains(
                    std::vector<Position>(
                        game.snake.begin() + 1,
                        game.snake.end()
                    ),
                    current
                )
            ) {
                std::cout << 'o';
            }
            else if (contains(game.foods, current)) {
                std::cout << '*';
            }
            else {
                bool isBigFood = false;

                for (const auto& big : game.bigFoods) {
                    if (
                        x >= big.first && x < big.first + 2 &&
                        y >= big.second && y < big.second + 2
                    ) {
                        isBigFood = true;
                        break;
                    }
                }

                std::cout << (isBigFood ? '@' : ' ');
            }
        }

        std::cout << '\n';
    }

    std::cout
        << "Score: " << game.score
        << " | Normal food: " << game.foods.size()
        << " | Big food: " << game.bigFoods.size()
        << " | dx: " << game.dx
        << " dy: " << game.dy
        << "\nWASD: Move | Q: Quit\n"
        << std::flush;
}

int main() {
    try {
        Terminal terminal;

        std::signal(SIGINT, handleSignal);
        std::signal(SIGTERM, handleSignal);

        std::random_device rd;
        std::mt19937 gen(rd());

        GameState game;

        game.snake = {
            {WIDTH / 2, HEIGHT / 2},
            {WIDTH / 2 - 1, HEIGHT / 2},
            {WIDTH / 2 - 2, HEIGHT / 2}
        };

        replenishNormalFood(game, gen);
        trySpawnBigFood(game, gen);

        bool running = true;
        bool lost = false;

        std::cout << "\033[2J\033[?25l";

        while (running && !stopRequested) {
            char input = readInput();

            switch (input) {
                case 'w':
                case 'W':
                    setDirection(game, 0, -1);
                    break;

                case 's':
                case 'S':
                    setDirection(game, 0, 1);
                    break;

                case 'a':
                case 'A':
                    setDirection(game, -1, 0);
                    break;

                case 'd':
                case 'D':
                    setDirection(game, 1, 0);
                    break;

                case 'q':
                case 'Q':
                    running = false;
                    continue;
            }

            MoveResult result = move(game);

            if (
                result == MoveResult::WallCollision ||
                result == MoveResult::SelfCollision
            ) {
                lost = true;
                running = false;
                break;
            }

            if (
                result == MoveResult::AteNormalFood ||
                result == MoveResult::AteBigFood
            ) {
                trySpawnBigFood(game, gen);
                replenishNormalFood(game, gen);
            }

            render(game);

            int delay = std::max(60, 150 - (game.score / 5) * 5);
            std::this_thread::sleep_for(
                std::chrono::milliseconds(delay)
            );
        }

        if (lost) {
            std::cout << "\nGame over. Final score: "
                      << game.score << '\n';
        }
        else if (!stopRequested) {
            std::cout << "\nGame exited. Final score: "
                      << game.score << '\n';
        }
    }
    catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
