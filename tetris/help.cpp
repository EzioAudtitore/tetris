// src/help.cpp
#include "help.hpp"
#include <algorithm>
#include <cmath>

using namespace sf;

UI::UI(const Font& font)
    : font_(font),
    title_(font, "Controls", 38),
    hint_(font, "Press R to return", 21), // 14 * 1.5 = 21
    panelAlpha_(0.f),
    targetAlpha_(0.f)
{
    title_.setStyle(Text::Bold);
    title_.setFillColor(Color::White);
    hint_.setFillColor(Color(240, 240, 240, 210));

    // black translucent overlay
    panel_.setFillColor(Color(0, 0, 0, 0)); // alpha animated in update()
    panel_.setOutlineColor(Color(255, 255, 255, 30));
    panel_.setOutlineThickness(0.f);

    setFeatureList({
        "Num1     : Hold piece",
        "Num2     : Pause / Resume",
        "R           : Toggle help",
        "Up Arrow    : Rotate piece",
        "Left / Right: Move piece",
        "Down        : Soft drop",
        "Space       : Hard drop"
        });
}

void UI::setFeatureList(const std::vector<std::string>& features) {
    lines_.clear();
    unsigned int charSize = 28;
    for (const auto& s : features) {
        Text t(font_, s, charSize);
        t.setFillColor(Color::White);
        t.setStyle(Text::Bold);
        lines_.push_back(std::move(t));
    }
}

void UI::setVisible(bool v) {
    targetAlpha_ = v ? 160.f : 0.f; // fade in/out
}

void UI::update(float dt) {
    // smooth fade of panel
    float speed = 300.f;
    if (panelAlpha_ < targetAlpha_)
        panelAlpha_ = std::min(panelAlpha_ + speed * dt, targetAlpha_);
    else if (panelAlpha_ > targetAlpha_)
        panelAlpha_ = std::max(panelAlpha_ - speed * dt, targetAlpha_);

    panel_.setFillColor(Color(0, 0, 0, static_cast<int>(panelAlpha_)));

    // pulsing alpha for all text
    static float pulse = 0.f;
    pulse += dt * 1.2f;
    float alpha = 200.f + 40.f * std::sin(pulse); // 160–240 approx

    int finalAlpha = static_cast<int>(std::clamp(alpha, 80.f, 255.f));

    // Apply alpha to title
    Color titleC = title_.getFillColor();
    title_.setFillColor(Color(titleC.r, titleC.g, titleC.b, finalAlpha));

    // Apply alpha to all lines
    for (auto& line : lines_) {
        Color c = line.getFillColor();
        line.setFillColor(Color(c.r, c.g, c.b, finalAlpha));
    }

    // Apply alpha to hint
    Color hintC = hint_.getFillColor();
    hint_.setFillColor(Color(hintC.r, hintC.g, hintC.b, finalAlpha));
}

void UI::render(RenderWindow& window) {
    Vector2u ws = window.getSize();
    panel_.setSize({ float(ws.x), float(ws.y) });
    panel_.setPosition({ 0.f, 0.f });
    window.draw(panel_);

    if (panelAlpha_ < 1.f) return;

    // Title at top center
    Text title = title_;
    FloatRect tb = title.getLocalBounds();
    title.setOrigin({ tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f });
    title.setPosition({ float(ws.x) / 2.f, 80.f });
    window.draw(title);

    // Feature lines with fixed palette: White, Magenta, Yellow, Cyan
    std::vector<Color> palette = { Color::White, Color::Magenta, Color::Yellow, Color::Cyan };
    float spacing = 42.f;
    float totalH = float(lines_.size() - 1) * spacing;
    float startY = float(ws.y) / 2.f - totalH / 2.f;

    for (size_t i = 0; i < lines_.size(); ++i) {
        Text t = lines_[i];
        t.setFillColor(Color(palette[i % palette.size()].r,
            palette[i % palette.size()].g,
            palette[i % palette.size()].b,
            t.getFillColor().a)); // keep pulsing alpha

        FloatRect lb = t.getLocalBounds();
        t.setOrigin({ lb.position.x + lb.size.x / 2.f, lb.position.y + lb.size.y / 2.f });
        t.setPosition({ float(ws.x) / 2.f, startY + float(i) * spacing });
        window.draw(t);
    }

    // Hint at bottom center (50% larger)
    Text h = hint_;
    FloatRect hb = h.getLocalBounds();
    h.setOrigin({ hb.position.x + hb.size.x / 2.f, hb.position.y + hb.size.y / 2.f });
    h.setPosition({ float(ws.x) / 2.f, float(ws.y) - 80.f });
    window.draw(h);
}
