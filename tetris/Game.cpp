// src/Game.cpp
#include "Game.hpp"
#include <cstdlib>
#include <ctime>
#include <limits>

using namespace sf;


Game::Game()
    : window_{ VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Tetris (Press space to pause)" }
    , currentPiece_{ static_cast<PieceType>(std::rand() % 7) }
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    auto& rm = ResourceManager::instance();
    rm.loadTexture("tiles", "images/tiles.(2x2).png");
    rm.loadTexture("background", "images/background.(2x2).png");
    rm.loadTexture("frame", "images/frame.(2x2).png");

    backgroundSprite_ = std::make_unique<Sprite>(rm.getTexture("background"));
    frameSprite_ = std::make_unique<Sprite>(rm.getTexture("frame"));
    tileSprite_ = std::make_unique<Sprite>(rm.getTexture("tiles"));
}

void Game::run() {
    float timer = 0.f;
    int   dx = 0;
    bool  rotate = false;
    Clock clock;

    while (window_.isOpen()) {
        window_.handleEvents(
            [&](const Event::Closed&) {
                window_.close();
            },
            [&](const Event::KeyPressed& key) {
                if (key.scancode == Keyboard::Scancode::Space) {
                    paused_ = !paused_;
                    return;
                }
                if (!paused_) {
                    if (key.scancode == Keyboard::Scancode::Up)    rotate = true;
                    else if (key.scancode == Keyboard::Scancode::Left)  dx = -1;
                    else if (key.scancode == Keyboard::Scancode::Right) dx = +1;
                }
            }
        );

        float delay = paused_
            ? std::numeric_limits<float>::infinity()
            : (Keyboard::isKeyPressed(Keyboard::Scancode::Down) ? 0.05f : 0.3f);

        timer += clock.restart().asSeconds();
        if (timer > delay) {
            if (!paused_) {
                currentPiece_.move(0, 1);
                if (!board_.isValid(currentPiece_)) {
                    currentPiece_.move(0, -1);
                    board_.lock(currentPiece_);
                    currentPiece_ = Tetromino{ static_cast<PieceType>(std::rand() % 7) };
                }
            }
            timer = 0.f;
        }

        if (!paused_ && dx) {
            currentPiece_.move(dx, 0);
            if (!board_.isValid(currentPiece_))
                currentPiece_.move(-dx, 0);
        }

        if (!paused_ && rotate) {
            currentPiece_.rotateCW();
            if (!board_.isValid(currentPiece_))
                currentPiece_.undoRotate();
        }

        dx = 0;
        rotate = false;

        render();
    }
}

void Game::render() {
    window_.clear(Color::White);
    window_.draw(*backgroundSprite_);

    board_.draw(window_, *tileSprite_);

    for (auto const& c : currentPiece_.blocks()) {
        IntRect rect({(currentPiece_.colorIndex() + 1) * CELL, 0}, {CELL, CELL });
        tileSprite_->setTextureRect(rect);
        auto p = currentPiece_.position();
        tileSprite_->setPosition({
            float((p.x + c.x) * CELL + OFFSET_X),
            float((p.y + c.y) * CELL + OFFSET_Y)
            });
        window_.draw(*tileSprite_);
    }
    if (paused_) {
        RectangleShape dim;
        dim.setSize({ float(WINDOW_WIDTH), float(WINDOW_HEIGHT) });
        dim.setFillColor(sf::Color(0, 0, 0, 50));
        window_.draw(dim);
    }
    window_.draw(*frameSprite_);
    window_.display();
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    Game game;
    game.run();
}