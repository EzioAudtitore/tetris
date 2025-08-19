//include/Game.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <optional>
#include "Board.hpp"
#include "Tetromino.hpp"
#include "ResourceManager.hpp"

using namespace sf;

class Game {
public:
    Game();
    void run();
    void initializeGame();
    void drawGhost();
    void endGame();

private:
    void render();

    static constexpr unsigned WINDOW_WIDTH = 640;
    static constexpr unsigned WINDOW_HEIGHT = 960;

    RenderWindow window_;
    Board board_;
    Tetromino currentPiece_;
    Tetromino holdPiece_;

    std::unique_ptr<Sprite> backgroundSprite_;
    std::unique_ptr<Sprite> frameSprite_;
    std::unique_ptr<Sprite> tileSprite_;

    Clock dropClock_;

    const float defaultDropDelay_ = 0.4f;
    float dropDelay_ = defaultDropDelay_;

    bool paused_ = false;
    bool gameOver_ = false;
    float resetTimer_ = 0.f;

    int score = 0;
    int level = 1;
    void speedup() {
        dropDelay_ *= 0.999f;
    }
    bool helpShown = false;

    std::optional<Sound> overTrack;
    Music* mainTrack = nullptr;
    Font serif;
};
