// include/Board.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include "Tetromino.hpp"

static constexpr int ROWS = 20;
static constexpr int COLS = 10;
static constexpr int CELL = 36;
static constexpr int OFFSET_X = 56;
static constexpr int OFFSET_Y = 62;

class Board {
public:
    Board();
    bool step(Tetromino& t, bool fastDrop);
    bool isValid(const Tetromino& t) const;
    void lock(const Tetromino& t);
    void clearLines();
    void draw(sf::RenderWindow& win, const sf::Sprite& tileSprite) const;

private:
    int grid[ROWS][COLS];
};
