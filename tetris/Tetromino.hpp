// include/Tetromino.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <array>


struct Point { int x, y; };

enum class PieceType { I, Z, S, T, L, J, O };

/// Represents a single falling Tetris piece.
class Tetromino {
    friend class Board;
public:
    Tetromino(PieceType t);
    void move(int dx, int dy);
    void rotateCW();
    void undoRotate();

    const std::array<Point, 4>& blocks()   const { return cells; }
    Point position() const{ return pos; }
    int colorIndex() const { return colorIdx; }

private:
    void buildShape(PieceType t);

    std::array<Point, 4> cells;
    std::array<Point, 4> savedCells;
    PieceType type;
    Point pos;
    int colorIdx;

    static const std::array<std::array<int, 4>, 7> FIGURES;
};