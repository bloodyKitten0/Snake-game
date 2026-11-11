#include "game_logic.hpp"
#include <cassert>
#include <iostream>
#include <random>

using namespace Snake;

void testNormalMovement() {
    GameState game;
    game.snake = {{10, 10}, {9, 10}, {8, 10}};

    MoveResult result = move(game);

    assert(result == MoveResult::Moved);
    assert(game.snake.front() == Position(11, 10));
    assert(game.snake.size() == 3);
}

void testWallCollision() {
    GameState game;
    game.snake = {{1, 5}, {2, 5}, {3, 5}};
    game.dx = -1;
    game.dy = 0;

    MoveResult result = move(game);

    assert(result == MoveResult::WallCollision);
    assert(game.snake.front() == Position(1, 5));
}

void testSelfCollision() {
    GameState game;

    game.snake = {
        {5, 5},
        {5, 6},
        {4, 6},
        {4, 5},
        {3, 5}
    };

    game.dx = -1;
    game.dy = 0;

    MoveResult result = move(game);

    assert(result == MoveResult::SelfCollision);
}

void testMovingIntoVacatedTail() {
    GameState game;

    game.snake = {
        {5, 5},
        {5, 6},
        {4, 6},
        {4, 5}
    };

    game.dx = -1;
    game.dy = 0;

    MoveResult result = move(game);

    assert(result == MoveResult::Moved);
    assert(game.snake.front() == Position(4, 5));
    assert(game.snake.size() == 4);
}

void testNormalFoodConsumption() {
    GameState game;

    game.snake = {{5, 5}, {4, 5}, {3, 5}};
    game.foods = {{6, 5}};

    MoveResult result = move(game);

    assert(result == MoveResult::AteNormalFood);
    assert(game.foods.empty());
    assert(game.score == 1);
    assert(game.growthPending == 1);
}

void testBigFoodConsumption() {
    GameState game;

    game.snake = {{5, 5}, {4, 5}, {3, 5}};
    game.bigFoods = {{6, 5}};

    MoveResult result = move(game);

    assert(result == MoveResult::AteBigFood);
    assert(game.bigFoods.empty());
    assert(game.score == 4);
    assert(game.growthPending == 4);
}

void testBigFoodPlacement() {
    GameState game;

    game.snake = {{5, 5}, {4, 5}, {3, 5}};

    assert(canPlaceBigFood(game, 10, 10));
    assert(!canPlaceBigFood(game, 5, 5));
    assert(!canPlaceBigFood(game, 0, 0));
    assert(!canPlaceBigFood(game, WIDTH - 2, 10));
}

void testFoodGeneration() {
    GameState game;
    game.snake = {{10, 10}, {9, 10}, {8, 10}};

    std::mt19937 gen(12345);

    replenishNormalFood(game, gen);

    assert(game.foods.size() == MAX_NORMAL_FOOD);

    for (const Position& food : game.foods) {
        assert(!contains(game.snake, food));
    }
}

void testNormalFoodLimit() {
    GameState game;
    game.snake = {{10, 10}, {9, 10}, {8, 10}};

    std::mt19937 gen(12345);

    replenishNormalFood(game, gen);
    replenishNormalFood(game, gen);

    assert(game.foods.size() == MAX_NORMAL_FOOD);
}

void testDirectionReversal() {
    GameState game;
    game.snake = {{10, 10}, {9, 10}, {8, 10}};

    setDirection(game, -1, 0);

    assert(game.dx == 1);
    assert(game.dy == 0);

    setDirection(game, 0, -1);

    assert(game.dx == 0);
    assert(game.dy == -1);
}

int main() {
    testNormalMovement();
    testWallCollision();
    testSelfCollision();
    testMovingIntoVacatedTail();
    testNormalFoodConsumption();
    testBigFoodConsumption();
    testBigFoodPlacement();
    testFoodGeneration();
    testNormalFoodLimit();
    testDirectionReversal();

    std::cout << "All tests passed.\n";

    return 0;
}
