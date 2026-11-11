#pragma once
#include <algorithm>
#include <random>
#include <utility>
#include <vector>

namespace Snake {

constexpr int HEIGHT = 20;
constexpr int WIDTH = 40;
constexpr int MAX_NORMAL_FOOD = 15;

using Position = std::pair<int, int>;

struct GameState {
    std::vector<Position> snake;
    std::vector<Position> foods;
    std::vector<Position> bigFoods;

    int dx = 1;
    int dy = 0;

    int growthPending = 0;
    int score = 0;
};

enum class MoveResult {
    Moved,
    AteNormalFood,
    AteBigFood,
    WallCollision,
    SelfCollision
};

inline bool contains(
    const std::vector<Position>& positions,
    const Position& target
) {
    return std::find(
        positions.begin(),
        positions.end(),
        target
    ) != positions.end();
}

inline bool occupiedByFood(
    const GameState& game,
    const Position& position
) {
    if (contains(game.foods, position)) {
        return true;
    }

    for (const auto& big : game.bigFoods) {
        int x = big.first;
        int y = big.second;

        if (
            position.first >= x &&
            position.first < x + 2 &&
            position.second >= y &&
            position.second < y + 2
        ) {
            return true;
        }
    }

    return false;
}

inline bool occupiedBySnake(
    const GameState& game,
    const Position& position
) {
    return contains(game.snake, position);
}

inline bool canPlaceBigFood(
    const GameState& game,
    int x,
    int y
) {
    if (x < 1 || y < 1 || x + 1 >= WIDTH - 1 ||
        y + 1 >= HEIGHT - 1) {
        return false;
    }

    for (int row = y; row < y + 2; ++row) {
        for (int col = x; col < x + 2; ++col) {
            Position cell{col, row};

            if (
                occupiedBySnake(game, cell) ||
                occupiedByFood(game, cell)
            ) {
                return false;
            }
        }
    }

    return true;
}

inline bool generateNormalFood(
    GameState& game,
    std::mt19937& gen
) {
    if (game.foods.size() >= MAX_NORMAL_FOOD) {
        return false;
    }

    std::vector<Position> available;

    for (int y = 1; y < HEIGHT - 1; ++y) {
        for (int x = 1; x < WIDTH - 1; ++x) {
            Position cell{x, y};

            if (
                !occupiedBySnake(game, cell) &&
                !occupiedByFood(game, cell)
            ) {
                available.push_back(cell);
            }
        }
    }

    if (available.empty()) {
        return false;
    }

    std::uniform_int_distribution<> choice(
        0,
        static_cast<int>(available.size()) - 1
    );

    game.foods.push_back(available[choice(gen)]);
    return true;
}

inline void replenishNormalFood(
    GameState& game,
    std::mt19937& gen
) {
    while (game.foods.size() < MAX_NORMAL_FOOD) {
        if (!generateNormalFood(game, gen)) {
            break;
        }
    }
}

inline bool trySpawnBigFood(
    GameState& game,
    std::mt19937& gen
) {
    if (!game.bigFoods.empty()) {
        return false;
    }

    std::uniform_int_distribution<> chance(1, 100);

    if (chance(gen) > 4) {
        return false;
    }

    std::vector<Position> available;

    for (int y = 1; y < HEIGHT - 2; ++y) {
        for (int x = 1; x < WIDTH - 2; ++x) {
            if (canPlaceBigFood(game, x, y)) {
                available.push_back({x, y});
            }
        }
    }

    if (available.empty()) {
        return false;
    }

    std::uniform_int_distribution<> choice(
        0,
        static_cast<int>(available.size()) - 1
    );

    game.bigFoods.push_back(available[choice(gen)]);
    return true;
}

inline MoveResult move(GameState& game) {
    Position next{
        game.snake.front().first + game.dx,
        game.snake.front().second + game.dy
    };

    if (
        next.first <= 0 ||
        next.first >= WIDTH - 1 ||
        next.second <= 0 ||
        next.second >= HEIGHT - 1
    ) {
        return MoveResult::WallCollision;
    }

    // The tail vacates its cell on this move unless growth is pending.
    std::size_t bodyEnd = game.snake.size();

    if (game.growthPending == 0 && bodyEnd > 0) {
        --bodyEnd;
    }

    for (std::size_t i = 0; i < bodyEnd; ++i) {
        if (game.snake[i] == next) {
            return MoveResult::SelfCollision;
        }
    }

    game.snake.insert(game.snake.begin(), next);

    if (game.growthPending > 0) {
        --game.growthPending;
    } else {
        game.snake.pop_back();
    }

    auto normal = std::find(
        game.foods.begin(),
        game.foods.end(),
        next
    );

    if (normal != game.foods.end()) {
        game.foods.erase(normal);
        game.growthPending += 1;
        game.score += 1;
        return MoveResult::AteNormalFood;
    }

    for (auto it = game.bigFoods.begin();
         it != game.bigFoods.end(); ++it) {

        int x = it->first;
        int y = it->second;

        if (
            next.first >= x && next.first < x + 2 &&
            next.second >= y && next.second < y + 2
        ) {
            game.bigFoods.erase(it);
            game.growthPending += 4;
            game.score += 4;
            return MoveResult::AteBigFood;
        }
    }

    return MoveResult::Moved;
}

inline void setDirection(
    GameState& game,
    int dx,
    int dy
) {
    // Prevent reversing directly into the snake's own neck.
    if (dx == -game.dx && dy == -game.dy) {
        return;
    }

    game.dx = dx;
    game.dy = dy;
}

} 
