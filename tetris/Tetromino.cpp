// src/Tetromino.cpp
#include "Tetromino.hpp"

const std::array<std::array<int, 4>, 7> Tetromino::FIGURES = { {
    {{1,3,5,7}}, {{2,4,5,7}}, {{3,5,4,6}}, {{3,5,4,7}},
    {{2,3,5,7}}, {{3,5,7,6}}, {{2,3,4,5}}
} };

const std::array<sf::Color, 7> Tetromino::COLORS = {
    sf::Color::Cyan, sf::Color::Red, sf::Color::Green,
    sf::Color(245,45,65), sf::Color::Yellow,
    sf::Color(76,181,245), sf::Color::Magenta
};

Tetromino::Tetromino(PieceType t)
    : type(t), pos{ 0,0 }
{
    buildShape(t);
}

void Tetromino::move(int dx, int dy) {
    pos.x += dx;
    pos.y += dy;
}

void Tetromino::buildShape(PieceType t) {
    const auto& fig = FIGURES[static_cast<int>(t)];
    for (int i = 0; i < 4; ++i)
        cells[i] = { fig[i] % 2, fig[i] / 2 };
}

void Tetromino::rotateCW() {
    savedCells = cells;
    auto pivot = cells[1];
    for (auto& c : cells) {
        int x = c.y - pivot.y;
        int y = c.x - pivot.x;
        c.x = pivot.x - x;
        c.y = pivot.y + y;
    }
}

void Tetromino::undoRotate() {
    cells = savedCells;
}
