// src/Board.cpp
#include "Board.hpp"

Board::Board() {
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
            grid[i][j] = 0;
}

bool Board::step(Tetromino& t, bool /*fastDrop*/) {
    t.move(0, 1);
    if (!isValid(t)) {
        t.move(0, -1);
        lock(t);
        return true;
    }
    return false;
}

bool Board::isValid(const Tetromino& t) const {
    for (auto const& c : t.blocks()) {
        int x = t.position().x + c.x;
        int y = t.position().y + c.y;
        if (x < 0 || x >= COLS || y >= ROWS) return false;
        if (y >= 0 && grid[y][x])     return false;
    }
    return true;
}

void Board::lock(const Tetromino& t) {
    for (auto const& c : t.blocks()) {
        int x = t.position().x + c.x;
        int y = t.position().y + c.y;
        if (y >= 0 && y < ROWS)
            grid[y][x] = t.colorIndex() + 1;
    }
    clearLines();
}

void Board::clearLines() {
    int k = ROWS - 1;
    for (int i = ROWS - 1; i >= 0; --i) {
        int count = 0;
        for (int j = 0; j < COLS; ++j) {
            if (grid[i][j]) ++count;
            grid[k][j] = grid[i][j];
        }
        if (count < COLS) --k;
    }
}

void Board::draw(sf::RenderWindow& win, const sf::Sprite& tileSprite) const {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (grid[i][j]) {
                sf::Sprite cell = tileSprite;
                cell.setTextureRect({ { (grid[i][j] - 1) * CELL, 0 }, { CELL, CELL } });
                cell.setPosition({
                    float(j * CELL + OFFSET_X),
                    float(i * CELL + OFFSET_Y)
                    });
                win.draw(cell);
            }
        }
    }
}
