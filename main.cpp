#include <iostream>
#include <vector>
#include <termios.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <random>
#include <utility>
#include <system_error>
#include <cerrno>
#include <cstring>

const int h = 20;
const int w = 40;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> loch(1, h - 2);
std::uniform_int_distribution<> locw(1, w - 2);

bool readInput(char& key) {
    return read(STDIN_FILENO, &key, 1) == 1;
}
class terminal_set{
    private:
        termios originalTerminal;
    public:
        terminal_set() {
            if (tcgetattr(STDIN_FILENO, &originalTerminal) == -1) {
                throw std::system_error(errno, std::generic_category(), "Failed to get terminal attributes");
            }
            termios newTerminal = originalTerminal;
            newTerminal.c_lflag &= ~(ICANON | ECHO);
            newTerminal.c_cc[VMIN] = 0;
            newTerminal.c_cc[VTIME] = 0;
            if (tcsetattr(STDIN_FILENO, TCSANOW, &newTerminal) == -1) {
                throw std::system_error(errno, std::generic_category(), "Failed to set terminal attributes");
            }
        }
        ~terminal_set() noexcept {
            if (tcsetattr(STDIN_FILENO, TCSANOW, &originalTerminal) == -1) {
                std::cerr << "Failed to restore terminal attributes: " << std::strerror(errno) << "\n";
            }
        }

        terminal_set(const terminal_set&) = delete;
        terminal_set& operator=(const terminal_set&) = delete;

};

void direction(char dir, int& dx, int& dy) {
    switch(dir) {
        case 'w':
            dx = 0;
            dy = -1;
            break;
        case 's':
            dx = 0;
            dy = 1;
            break;
        case 'a':
            dx = -1;
            dy = 0;
            break;
        case 'd':
            dx = 1;
            dy = 0;
            break;
    }
}

void movement(std::vector<std::pair<int,int>>& snake, int dx,int dy){
    for(int i = snake.size()-1 ; i > 0 ; i--){
        snake[i] = snake[i-1];
    }
    snake[0].first += dx;
    snake[0].second += dy;
}

void generateFood(
    std::pair<int,int>& food,
    const std::vector<std::pair<int,int>>& snake
) {
    int x;
    int y;
    bool occupied;
    do {
        x = locw(gen);
        y = loch(gen);
        occupied = false;
        for (const auto& segment : snake) {
            if (segment.first == x && segment.second == y) {
                occupied = true;
                break;
            }
        }
    } while (occupied);
    food = {x, y};
}

bool checkFoodCollision(
    const std::vector<std::pair<int,int>>& snake,
    const std::pair<int,int>& food
) {
    return snake[0] == food;
}

bool checkWallCollision(
    const std::vector<std::pair<int,int>>& snake
) {
    int x = snake[0].first;
    int y = snake[0].second;

    return x <= 0 || x >= w - 1 ||
           y <= 0 || y >= h - 1;
}

bool checkSelfCollision(
    const std::vector<std::pair<int,int>>& snake
) {
    for (std::size_t i = 1; i < snake.size(); i++) {
        if (snake[0] == snake[i]) {
            return true;
        }
    }

    return false;
}

void render(
    std::vector<std::vector<char>>& map,
    const std::vector<std::pair<int,int>>& snake,
    const std::pair<int, int>& food
){
    for(std::size_t i = 0; i< map.size(); i++){
        for(std::size_t j = 0; j < map[i].size(); j++){
            if(i == 0 || i == h-1 || j==0 || j==w-1){
                map[i][j] = '#';
            }
            else{
                map[i][j] = ' ';
            }
            }
        }
        for(std::size_t i = 0;i<snake.size();i++){
            map[snake[i].second][snake[i].first]=(i==0)? '6':'Q';
        }

        map[food.second][food.first] = '@';

        for(std::size_t i = 0; i< map.size(); i++){
            for(std::size_t j = 0; j < map[i].size(); j++){
                std::cout << map[i][j];
            }
            std::cout << "\n";
        }
}

int main() {
    std::vector<std::vector<char>> map(h, std::vector<char>(w, ' '));
    std::vector<std::pair<int,int>> snake;
    std::pair<int, int> food;
    snake.push_back({20, 10});
    snake.push_back({19, 10});
    snake.push_back({18, 10});
    generateFood(food, snake);
    int dx = 1;
    int dy = 0;
    terminal_set Let_the_show_begin;
    bool running = true;

    while (running) {
        char key;
        if (readInput(key)) {
            if (key == 'q') {
                running = false;
                break;
            }
            direction(key, dx, dy);
        }
        auto oldTail = snake.back();
        movement(snake, dx, dy);
        if (checkFoodCollision(snake, food)) {
            snake.push_back(oldTail);
            generateFood(food, snake);
        }
        std::cout << "\033[H" << std::flush;
        render(map, snake, food);
        if (checkWallCollision(snake) || checkSelfCollision(snake)) {
            running = false;
            break;
        }
        std::this_thread::sleep_for(
            std::chrono::milliseconds(150)
        );
        std::cout << "dx: " << dx << " dy: " << dy << '\n';
    }
    return 0;
}