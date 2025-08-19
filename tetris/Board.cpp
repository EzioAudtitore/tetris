//src/Board.cpp
#include "Board.hpp"

Board::Board() {
    reset();
}

bool Board::step(Tetromino& t, bool) {
    t.move(0, 1);
    if (!isValid(t)) {
        t.move(0, -1);
        lock(t);
        return true;
    }
    return false;
}

bool Board::isValid(Tetromino const& t) const {
    auto pos = t.position();
    for (auto const& c : t.blocks()) {
        int x = pos.x + c.x;
        int y = pos.y + c.y;
        if (x < 0 || x >= COLS || y >= ROWS) return false;
        if (y >= 0 && grid[y][x] != 0) return false;
    }
    return true;
}

void Board::lock(Tetromino const& t) {
    auto pos = t.position();
    int color = t.colorIndex();
    for (auto const& c : t.blocks()) {
        int x = pos.x + c.x;
        int y = pos.y + c.y;
        if (y >= 0 && y < ROWS && x >= 0 && x < COLS) {
            grid[y][x] = color;
        }
    }
    scheduleClear();
}

void Board::scheduleClear() {
    linesToClear_.clear();
    for (int i = 0; i < ROWS; ++i) {
        bool full = true;
        for (int j = 0; j < COLS; ++j) {
            if (grid[i][j] == 0) { full = false; break; }
        }
        if (full) linesToClear_.push_back(i);
    }

    if (!linesToClear_.empty()) {
        flashTimer_ = CLEAR_FLASH_DURATION;
    }
}

void Board::applyClear() {
    if (linesToClear_.empty()) return;

    std::sort(linesToClear_.begin(), linesToClear_.end());

    int writeRow = ROWS - 1;
    for (int readRow = ROWS - 1; readRow >= 0; --readRow) {
        if (std::binary_search(linesToClear_.begin(), linesToClear_.end(), readRow)) {
            continue;
        }
        if (writeRow != readRow) {
            for (int c = 0; c < COLS; ++c) {
                grid[writeRow][c] = grid[readRow][c];
            }
        }
        --writeRow;
    }

    for (int r = writeRow; r >= 0; --r) {
        for (int c = 0; c < COLS; ++c) grid[r][c] = 0;
    }

    linesToClear_.clear();
    flashTimer_ = 0.f;
}

void Board::update(float dt) {
    if (flashTimer_ > 0.f) {
        flashTimer_ -= dt;
        if (flashTimer_ <= 0.f) {
            applyClear();
        }
    }
}

bool Board::isClearing() const {
    return flashTimer_ > 0.0f;
}

void Board::reset() {
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
            grid[i][j] = 0;
    linesToClear_.clear();
    flashTimer_ = 0.f;
}

int rows() {
    return ROWS;
}

int cols() {
    return COLS;
}

void Board::draw(sf::RenderWindow& window, const sf::Sprite& tileSprite) const {
    sf::Sprite sprite = tileSprite;

    for (int r = 0; r < rows(); ++r) {
        for (int c = 0; c < cols(); ++c) {
            int val = grid[r][c];
            if (val == 0) continue;
            sprite.setTextureRect(sf::IntRect({ val * CELL, 0 }, { CELL, CELL }));
            sprite.setPosition({
                float(OFFSET_X + c * CELL),
                float(OFFSET_Y + r * CELL)
                });
            window.draw(sprite);
        }
    }

    if (flashTimer_ > 0.f && !linesToClear_.empty()) {
        sf::RectangleShape overlay(sf::Vector2f(CELL * cols(), CELL));
        overlay.setFillColor(sf::Color(255, 255, 255, 80));
        for (int r : linesToClear_) {
            overlay.setPosition({
                float(OFFSET_X),
                float(OFFSET_Y + r * CELL)
                });
            window.draw(overlay);
        }
    }
}

void Board::drawHoldBox(sf::RenderWindow& window, const sf::Font& font) const {
    constexpr int MAX_SHAPE_CELLS = 4;
    float boxSize = (MAX_SHAPE_CELLS + 1.f) * CELL * 0.8f; // 20% smaller box

    float holdX = float(OFFSET_X) + cols() * CELL + 20.f + 35.f;
    float holdY = float(OFFSET_Y) + (rows() * CELL - boxSize) / 2.f;

    sf::RectangleShape holdRect({ boxSize, boxSize });
    holdRect.setPosition({ holdX, holdY });
    holdRect.setFillColor(sf::Color(0, 0, 0, 100));

    holdRect.setOutlineThickness(5.f);
    holdRect.setOutlineColor(sf::Color(255, 255, 255, 120));
    window.draw(holdRect);

    sf::RectangleShape shadowRect({ boxSize - 6.f, boxSize - 6.f });
    shadowRect.setPosition({ holdX + 3.f, holdY + 3.f });
    shadowRect.setFillColor(sf::Color(0, 0, 0, 0));
    window.draw(shadowRect);

    sf::Text holdLabel(font, "Hold", 40);
    holdLabel.setStyle(sf::Text::Bold);
    holdLabel.setFillColor(sf::Color::White);
    sf::FloatRect lblBounds = holdLabel.getLocalBounds();
    holdLabel.setOrigin({
        lblBounds.position.x + lblBounds.size.x / 2.f,
        lblBounds.position.y + lblBounds.size.y / 2.f
        });
    holdLabel.setPosition({ holdX + boxSize / 2.f, holdY - 40.f });
    window.draw(holdLabel);
}

void Board::drawHoldTetromino(sf::RenderWindow& window, const sf::Sprite& tileSprite, const Tetromino& holdPiece) const {
    auto blocks = holdPiece.blocks();
    if (blocks.empty()) return;

    constexpr int MAX_SHAPE_CELLS = 4;
    float boxSize = (MAX_SHAPE_CELLS + 1.f) * CELL * 0.9f; // smaller box
    float holdX = float(OFFSET_X) + cols() * CELL + 20.f + 45.f;
    float holdY = float(OFFSET_Y) + (rows() * CELL - boxSize) / 2.f;

    int minX = blocks[0].x, maxX = blocks[0].x;
    int minY = blocks[0].y, maxY = blocks[0].y;
    for (auto const& b : blocks) {
        minX = std::min(minX, b.x);
        maxX = std::max(maxX, b.x);
        minY = std::min(minY, b.y);
        maxY = std::max(maxY, b.y);
    }
    int w = maxX - minX + 1;
    int h = maxY - minY + 1;

    float originX = holdX + (boxSize - w * CELL * 0.9f) / 2.f - float(minX * CELL * 0.9f);
    float originY = holdY + (boxSize - h * CELL * 0.9f) / 2.f - float(minY * CELL * 0.9f);

    sf::Sprite sprite = tileSprite;
    sprite.setTextureRect(sf::IntRect({ holdPiece.colorIndex() * CELL, 0 }, { CELL, CELL }));
    sprite.setScale({ 0.9f, 0.9f }); // 10% smaller tetromino
    for (auto const& c : blocks) {
        sprite.setPosition({
            originX + float(c.x * CELL * 0.9f),
            originY + float(c.y * CELL * 0.9f)
            });
        window.draw(sprite);
    }
}
