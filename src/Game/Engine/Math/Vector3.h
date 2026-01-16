#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <array>
#include "../Core/Utility.h"

// This a bare bones Vector3 class with some basic methods and operation overloads.
class Vector3
{
public:
    static Vector3 ZERO;
    static Vector3 ONE;
    static Vector3 UP;
    static Vector3 DOWN;
    static Vector3 RIGHT;
    static Vector3 LEFT;
    static Vector3 FORWARD;
    static Vector3 BACKWARD;

public:
    // w is a 4th term needed to perform sensible matrix vector multiplication
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
    float w = 1.f;

    // Constructors
    Vector3()
        : x(0.f), y(0.f), z(0.f), w(1.f)
    {}
    Vector3(float x, float y, float z, float w = 1.f)
        : x(x), y(y), z(z), w(w)
    {}

    // Operator overloads
    Vector3 operator+(const Vector3& b) const
    {
        return Vector3(x + b.x, y + b.y, z + b.z);
    }
    Vector3& operator+=(const Vector3& b)
    {
        this->x += b.x;
        this->y += b.y;
        this->z += b.z;
        return *this;
    }

    Vector3 operator-(const Vector3& b) const
    {
        return Vector3(x - b.x, y - b.y, z - b.z);
    }
    Vector3& operator-=(const Vector3& b)
    {
        this->x -= b.x;
        this->y -= b.y;
        this->z -= b.z;
        return *this;
    }

    // Scalar multiplication
    Vector3 operator*(const float scalar) const
    {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    Vector3 operator*(const Vector3& vector) const
    {
        return Vector3(x * vector.x, y * vector.y, z * vector.z);
    }
    Vector3& operator*=(const float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    // Scalar division
    Vector3 operator/(const float scalar) const
    {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
    Vector3& operator/=(const float scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    std::array<float, 3> GetArray() const
    {
        std::array<float, 3> arr = {x, y, z};
        return arr;
    }

    float GetDotProduct(const Vector3& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3 GetCrossProduct(const Vector3& other) const
    {
        Vector3 v;
        v.x = y * other.z - z * other.y;
        v.y = z * other.x - x * other.z;
        v.z = x * other.y - y * other.x;
        return v;
    }

    Vector3 GetNormal() const
    {
        // Vector3 normal = *this;
        // normal.Normalize();
        // return normal;
        float magnitude = GetMagnitude();
        return Vector3{ x / magnitude, y / magnitude, z / magnitude };
    }

    // Useful methods
    void Zero()
    {
        x = 0.f;
        y = 0.f;
        z = 0.f;
    }

    void Invert()
    {
        x = -x;
        y = -y;
        z = -z;
    }

    float GetMagnitude() const
    {
        return std::sqrtf(x * x + y * y + z * z);
    }

    // Useful for comparisons since it is faster than GetMagnitude()
    float GetMagnitudeSquared() const
    {
        return x*x + y*y + z*z;
    }

    Vector3 GetProjection(const Vector3& direction) const
    {
        float dot = GetDotProduct(direction);
        float magSq = direction.GetMagnitudeSquared();
        return direction * (dot / magSq);
    }

    bool Equals(const Vector3& other) const
    {
        // return (x == other.x && y == other.y && z == other.z);
        return (CMP_FLOAT(x, other.x) && CMP_FLOAT(y, other.y) && CMP_FLOAT(z, other.z));
    }

    void Normalize()
    {
        float magnitude = GetMagnitude();
        // if (magnitude > 0)
        // {
        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
        // }
    }

    void AddScaledVector(const Vector3& vector, float scale)
    {
        x += vector.x * scale;
        y += vector.y * scale;
        z += vector.z * scale;
    }

    Vector3 GetPerpendicularVector() const
    {
        // https://stackoverflow.com/a/41285544
        Vector3 result = Vector3(std::fabsf(x), std::fabsf(y), std::fabsf(z));
        if (result.x < result.y)
        {
            if (result.x < result.z)
                result = Vector3(0, -z, y);
            else
                result = Vector3(-y, x, 0);
        }
        else
        {
            if (result.y < result.z)
                result = Vector3(z, 0, -x);
            else
                result = Vector3(-y, x, 0);
        }

        return result;
    }
            

    std::string GetString() const
    {
        std::string pos = "X = " + std::to_string(x) + ", Y = " + std::to_string(y) + ", Z = " + std::to_string(z);
        return pos;
    }
};
