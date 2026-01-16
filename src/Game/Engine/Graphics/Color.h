#pragma once

#include <cstdint>
#include <string>

// Values only from 0-255
struct Color
{
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
    uint8_t a = 255;

    Color() = default;
    Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255)
        : r(r), g(g), b(b), a(a)
    {}

    bool operator ==(const Color& c)
    {
        return (r == c.r && g == c.g && b == c.b && a == c.a);
    }

    void SetColorScale(float scale)
    {
        if (scale > 1.0f || scale < 0.0f)
        {
            //SDL_Log("Error: Color scale value must be between 0 and 1!");
        }
        else
        {
            r *= scale;
            g *= scale;
            b *= scale;
        }
    }

    // Set it to white then scale it
    void SetGrayScale(float scale)
    {
        r = 255;
        g = 255;
        b = 255;
        SetColorScale(scale);
    }

    float GetPercentageRed() const
    {
        return r / 255.f;
    }

    float GetPercentageGreen() const
    {
        return g / 255.f;
    }

    float GetPercentageBlue() const
    {
        return b / 255.f;
    }

    std::string PrintColor() const
    {
        return "R: " + std::to_string(r) + ", G: " + std::to_string(g) + ", B: " + std::to_string(b);
    }
};
