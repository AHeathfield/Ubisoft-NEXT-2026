#pragma once

#include <cmath>
#include <string>

// This a bare bones Vector2 class with some basic methods and operation overloads.
// Below there is also Vector2i which is the same but all integer based.
class Vector2
{
public:
    float x = 0.f;
    float y = 0.f;

    // Constructors
    Vector2()
        : x(0.f), y(0.f)
    {}
    Vector2(float x, float y)
        : x(x), y(y)
    {}

    // Operator overloads
    Vector2 operator+(const Vector2& b) const
    {
        return Vector2(x + b.x, y + b.y);
    }

    Vector2 operator-(const Vector2& b) const
    {
        return Vector2(x - b.x, y - b.y);
    }

    // Scalar multiplication
    Vector2 operator*(const float scalar) const
    {
        return Vector2(x * scalar, y * scalar);
    }

    // Scalar division
    Vector2 operator/(const float scalar) const
    {
        return Vector2(x / scalar, y / scalar);
    }

    // Useful methods
    void zero()
    {
        x = 0.f;
        y = 0.f;
    }

    void invert()
    {
        x = -x;
        y = -y;
    }

    void swap(Vector2& other)
    {
        std::swap(x, other.x);
        std::swap(y, other.y);
    }

    float getMagnitude() const
    {
        return std::sqrtf(x * x + y * y);
    }

    bool equals(const Vector2& other) const
    {
        return (x == other.x && y == other.y);
    }

    void normalize()
    {
        float magnitude = getMagnitude();
        if (magnitude > 0)
        {
            this->x = x / magnitude;
            this->y = y / magnitude;
        }
    }

    float getAngleRadians() const
    {
        return std::atanf(y / x);
    }

    void addScaledVector(const Vector2& vector, float scale)
    {
        x += vector.x * scale;
        y += vector.y * scale;
    }

    std::string GetString() const
    {
        std::string pos = "X = " + std::to_string(x) + ", Y = " + std::to_string(y);
        return pos;
    }
};


// Integer vector2!
class Vector2i
{
public:
    int x, y;

    // Constructors
    Vector2i()
        : x(0), y(0)
    {}
    Vector2i(int x, int y)
        : x(x), y(y)
    {}
    Vector2i(const Vector2& v)
        : x(std::round(v.x)), y(std::round(v.y))
    {}

    // Operator overloads
    Vector2i operator+(const Vector2i& b) const
    {
        return Vector2i(x + b.x, y + b.y);
    }

    Vector2i operator-(const Vector2i& b) const
    {
        return Vector2i(x - b.x, y - b.y);
    }

    // Scalar multiplication
    Vector2i operator*(const float scalar) const
    {
        return Vector2i(x * scalar, y * scalar);
    }

    // Scalar division
    Vector2i operator/(const float scalar) const
    {
        return Vector2i(x / scalar, y / scalar);
    }

    // Useful methods
    void zero()
    {
        x = 0;
        y = 0;
    }

    void invert()
    {
        x = -x;
        y = -y;
    }

    void swap(Vector2i& other)
    {
        std::swap(x, other.x);
        std::swap(y, other.y);
    }

    float getMagnitude() const
    {
        return std::sqrtf(x * x + y * y);
    }

    bool equals(const Vector2i& other) const
    {
        return (x == other.x && y == other.y);
    }

    void normalize()
    {
        float magnitude = getMagnitude();
        if (magnitude > 0)
        {
            this->x = x / magnitude;
            this->y = y / magnitude;
        }
    }

    float getAngleRadians() const
    {
        return std::atan(y / x);
    }

    void addScaledVector(const Vector2i& vector, int scale)
    {
        x += vector.x * scale;
        y += vector.y * scale;
    }

    std::string GetString() const
    {
        std::string pos = "X = " + std::to_string(x) + ", Y = " + std::to_string(y);
        return pos;
    }
};
