//include/help.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class UI {
public:
    UI(const sf::Font& font);

    // advance animations / pulses
    void update(float dt);

    // draw the pause/help overlay (now full-window)
    void render(sf::RenderWindow& window);

    // optionally change the controls text
    void setFeatureList(const std::vector<std::string>& features);

    // show/hide overlay with fade
    void setVisible(bool v);

private:
    const sf::Font& font_;

    sf::RectangleShape panel_;
    sf::Text title_;
    sf::Text hint_;
    std::vector<sf::Text> lines_;

    // fading state
    float panelAlpha_;   // current alpha
    float targetAlpha_;  // desired alpha
};
