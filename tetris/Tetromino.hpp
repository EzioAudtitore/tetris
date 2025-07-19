// include/Tetromino.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <array>

struct Point { int x, y; };

enum class PieceType { I, Z, S, T, L, J, O };

/// Represents a single falling Tetris piece.
class Tetromino {
public:
    Tetromino(PieceType t);
    void move(int dx, int dy);
    void rotateCW();
    void undoRotate();

    const std::array<Point, 4>& blocks()   const { return cells; }
    Point                       position() const { return pos; }
    int                         colorIndex() const { return static_cast<int>(type); }

private:
    void buildShape(PieceType t);

    std::array<Point, 4> cells;
    std::array<Point, 4> savedCells;
    PieceType           type;
    Point               pos;

    static const std::array<std::array<int, 4>, 7> FIGURES;
    static const std::array<sf::Color, 7>         COLORS;
};
