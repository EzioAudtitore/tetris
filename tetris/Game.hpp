// include/Game.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Board.hpp"
#include "Tetromino.hpp"
#include "ResourceManager.hpp"

class Game {
public:
    Game();
    void run();

private:
    void render();

    static constexpr unsigned WINDOW_WIDTH = 640;
    static constexpr unsigned WINDOW_HEIGHT = 960;

    sf::RenderWindow window_;
    Board            board_;
    Tetromino        currentPiece_;

    // Sprites constructed with textures to satisfy SFML 3.0
    std::unique_ptr<sf::Sprite> backgroundSprite_;
    std::unique_ptr<sf::Sprite> frameSprite_;
    std::unique_ptr<sf::Sprite> tileSprite_;

    sf::Clock dropClock_;
    float     dropDelay_ = 0.3f;

    bool      paused_ = false;
};
