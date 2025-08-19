// include/Board.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include "Tetromino.hpp"

using namespace sf;

static constexpr int ROWS = 20;
static constexpr int COLS = 10;
static constexpr int CELL = 36;
static constexpr int OFFSET_X = 56;
static constexpr int OFFSET_Y = 62;
static constexpr float CLEAR_FLASH_DURATION = 0.4f;

int rows();
int cols();

class Board {
public:
    Board();
    bool step(Tetromino& t, bool fastDrop);
    bool isValid(const Tetromino& t) const;
    void lock(const Tetromino& t);
    void draw(RenderWindow& win, const Sprite& tileSprite) const;
    void reset();
    Tetromino& respawn(Tetromino& t) {
        t.move((cols()-1)/2 - t.position().x, -t.position().y);
        return t;
    }
    void update(float dt);
    bool isClearing() const;
   
    void drawHoldBox(sf::RenderWindow& window, const sf::Font& font) const;
    void drawHoldTetromino(
        RenderWindow& window, 
        const Sprite& tileSprite, 
        const Tetromino& holdPiece) const;

    int lastCleared() const { return lastCleared_; }

private:
    int lastCleared_ = 0;
    int grid[ROWS][COLS];
    void scheduleClear();
    void applyClear();
    std::vector<int> linesToClear_;
    float flashTimer_ = CLEAR_FLASH_DURATION;
};