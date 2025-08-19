// src/Game.cpp
#include "ResourceManager.hpp"
#include "Game.hpp"
#include <cstdlib>
#include <ctime>
#include <limits>
#include <stdexcept>

#include "help.hpp" 

static UI* s_ui = nullptr;

Game::Game()
    : window_{ VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Tetris" }
    , currentPiece_{ static_cast<PieceType>(std::rand() % 7) }
    , holdPiece_{ static_cast<PieceType>(std::rand() % 7) }
    , helpShown{ false }
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    auto& rm = ResourceManager::instance();

    rm.loadTexture("tiles", "images/tiles.(2x2).png");
    rm.loadTexture("background", "images/background.(2x2).png");
    rm.loadTexture("frame", "images/frame.(2x2).png");

    backgroundSprite_ = std::make_unique<Sprite>(rm.getTexture("background"));
    frameSprite_ = std::make_unique<Sprite>(rm.getTexture("frame"));
    tileSprite_ = std::make_unique<Sprite>(rm.getTexture("tiles"));

    rm.loadSoundBuffer("gameOver", "tracks/gameOver.wav");
    overTrack.emplace(rm.getSoundBuffer("gameOver"));
    overTrack->setLooping(false);
    overTrack->setVolume(100.f);

    rm.createMusic("mainTrack", "tracks/mainTrack.ogg");
    mainTrack = &rm.getMusic("mainTrack");
    mainTrack->setLooping(true);

    if (!serif.openFromFile("fonts/serif.ttf")) {
        throw std::runtime_error("Failed to load font: fonts/serif.ttf");
    }

    if (!s_ui) {
        s_ui = new UI(serif);
    }
}

void Game::initializeGame() {
    if (overTrack) {
        overTrack->stop();
        overTrack->setPlayingOffset(Time::Zero);
    }

    board_.reset();
    currentPiece_ = Tetromino{ static_cast<PieceType>(std::rand() % 7) };
    paused_ = false;
    gameOver_ = false;
    dropDelay_ = defaultDropDelay_;
    helpShown = false;
if (s_ui) s_ui->setVisible(false);
}

void Game::drawGhost() {
    Tetromino ghost = currentPiece_;
    while (board_.isValid(ghost))
        ghost.move(0, 1);
    ghost.move(0, -1);
    bool topOut = (ghost.position().y < 0);
    for (auto const& c : ghost.blocks()) {
        IntRect rect({ ghost.colorIndex() * CELL, 0 }, { CELL, CELL });
        tileSprite_->setTextureRect(rect);
        tileSprite_->setColor(Color(255, 255, 255, 50));
        auto p = ghost.position();
        tileSprite_->setPosition({
            float((p.x + c.x) * CELL + OFFSET_X),
            float((p.y + c.y) * CELL + OFFSET_Y)
            });
        window_.draw(*tileSprite_);
    }
    tileSprite_->setColor(Color::White);
    if (topOut) {
        endGame();
    }
}

void Game::run() {
    if (mainTrack) mainTrack->play();

    float timer = 0.f;
    int dx = 0;
    bool rotate = false;
    Clock clock;

    while (window_.isOpen()) {
        bool hardDrop = false;
        window_.handleEvents(
            [&](const Event::Closed&) {
                window_.close();
            },
            [&]() {},
            [&](const Event::KeyPressed& key) {
                if (key.scancode == Keyboard::Scancode::R) {
                    helpShown = !helpShown;
                    if (s_ui) s_ui->setVisible(helpShown); 
                    if (helpShown) {
                        if (mainTrack) mainTrack->pause();
                    }
                    else {
                        if (!paused_ && mainTrack) mainTrack->play();
                    }
                    return;
                }

                if (key.scancode == Keyboard::Scancode::Num2) {
                    paused_ = !paused_;
                    if (gameOver_) {
                        gameOver_ = false;
                        initializeGame();
                        if (mainTrack) mainTrack->play();
                    }
                    if (!paused_) {
                        if (mainTrack) mainTrack->play();
                    }
                    else {
                        if (mainTrack) mainTrack->pause();
                    }
                    return;
                }

                if (key.scancode == Keyboard::Scancode::Num1) {
                    Tetromino temp = holdPiece_ ;
                    holdPiece_ = board_.respawn(currentPiece_);
                    currentPiece_ = board_.respawn(temp);
                }
                if (key.scancode == Keyboard::Scancode::N) {
                    initializeGame();
                }
                if (!paused_ && !helpShown && !(board_.isClearing())) {
                    if (key.scancode == Keyboard::Scancode::Up) rotate = true;
                    else if (key.scancode == Keyboard::Scancode::Left) dx = -1;
                    else if (key.scancode == Keyboard::Scancode::Right) dx = +1;
                    else if (key.scancode == Keyboard::Scancode::Space) hardDrop = true;
                }
            }
        );

        float delay = (paused_ || helpShown)
            ? std::numeric_limits<float>::infinity()
            : (Keyboard::isKeyPressed(Keyboard::Scancode::Down) ? dropDelay_ / 10 : dropDelay_);

        float dt = clock.restart().asSeconds();
        timer += dt;

        if (s_ui) s_ui->update(dt);

        board_.update(dt);
        if (timer > delay && !(board_.isClearing())) {
            currentPiece_.move(0, 1);
            if (!board_.isValid(currentPiece_)) {
                currentPiece_.move(0, -1);
                board_.lock(currentPiece_);
                if (!board_.isClearing())
                    currentPiece_ =
                    Tetromino{ static_cast<PieceType>(std::rand() % 7) };
            }
            timer = 0.f;
        }

        if (!paused_ && !helpShown && dx) {
            currentPiece_.move(dx, 0);
            if (!board_.isValid(currentPiece_))
                currentPiece_.move(-dx, 0);
        }

        if (!paused_ && !helpShown && rotate) {
            currentPiece_.rotateCW();
            if (!board_.isValid(currentPiece_))
                currentPiece_.undoRotate();
        }

        if (!paused_ && !helpShown && hardDrop) {
            while (board_.isValid(currentPiece_)) {
                currentPiece_.move(0, 1);
            }
            currentPiece_.move(0, -1);
            board_.lock(currentPiece_);
            speedup();
            if (!board_.isClearing())
                currentPiece_ = Tetromino{ static_cast<PieceType>(std::rand() % 7) };
            timer = 0.f;
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
    drawGhost();
    for (auto const& c : currentPiece_.blocks()) {
        IntRect rect({ currentPiece_.colorIndex() * CELL, 0 }, { CELL, CELL });
        tileSprite_->setTextureRect(rect);
        auto p = currentPiece_.position();
        tileSprite_->setPosition({
            float((p.x + c.x) * CELL + OFFSET_X),
            float((p.y + c.y) * CELL + OFFSET_Y)
            });
        window_.draw(*tileSprite_);
    }
    
    board_.drawHoldBox(window_, serif);
    board_.drawHoldTetromino(window_, *tileSprite_, holdPiece_);

    if (paused_) {
        RectangleShape dim;
        dim.setSize({ float(WINDOW_WIDTH), float(WINDOW_HEIGHT) });
        dim.setFillColor(Color(0, 0, 0, 50));
        window_.draw(dim);
    }

    if (helpShown) {
        if (s_ui) 
            s_ui->render(window_);
    }
    window_.display();
}

void Game::endGame() {
    if (gameOver_) return;
    gameOver_ = true;
    paused_ = true;
    if (mainTrack) mainTrack->stop();
    if (overTrack) {
        overTrack->setPlayingOffset(Time::Zero);
        overTrack->play();
    }
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    Game game;
    game.run();
    return 0;
}