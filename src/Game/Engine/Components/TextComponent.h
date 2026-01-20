#pragma once

#include <string>
#include "../Graphics/Color.h"

struct TextComponent
{
    std::string text = "";
    Color color = Color(255, 255, 255);

    TextComponent() = default;
    TextComponent(const std::string& text, const Color& color)
        : text(text),
        color(color)
    {}
};
