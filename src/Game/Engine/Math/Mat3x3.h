#pragma once

#include "../Math/Vector3.h"
#include <string>
#include <array>

// NOTE: This class does not contain the matrix tranformations since those are used in 4x4
struct Mat3x3
{
    // [row][col]
    float m[3][3] = { 0 };

    static Mat3x3 Identity()
    {
        Mat3x3 result;
        result.m[0][0] = 1.f;
        result.m[1][1] = 1.f;
        result.m[2][2] = 1.f;
        return result;
    }

    // Operator overloads
    Mat3x3 operator*(const Mat3x3& other)
    {
        Mat3x3 matrix;
        for (int c = 0; c < 3; c++)
        {
            for (int r = 0; r < 3; r++)
            {
                matrix.m[r][c] = m[r][0] * other.m[0][c] + m[r][1] * other.m[1][c] + m[r][2] * other.m[2][c];
            }
        }
        return matrix;
    }
    Mat3x3& operator*=(const Mat3x3& other)
    {
        // Just reusing the operator above
        *this = *this * other;
        return *this;
    }

    std::array<Vector3, 3> GetVector3Array() const
    {
        return std::array<Vector3, 3> {
            Vector3(m[0][0], m[0][1], m[0][2]),
            Vector3(m[1][0], m[1][1], m[1][2]),
            Vector3(m[2][0], m[2][1], m[2][2])
        };
    }

    
    std::array<float, 9> GetFloatArray() const
    {
        return std::array<float, 9> {
            m[0][0], m[0][1], m[0][2],
            m[1][0], m[1][1], m[1][2],
            m[2][0], m[2][1], m[2][2]
        };
    }

    Vector3 GetMultiplyVector3(const Vector3& i) const
    {
        Vector3 v;
        v.x = i.x * m[0][0] + i.y * m[1][0] + i.z * m[2][0];
        v.y = i.x * m[0][1] + i.y * m[1][1] + i.z * m[2][1];
        v.z = i.x * m[0][2] + i.y * m[1][2] + i.z * m[2][2];
        return v;
    }

    // Mat3x3 GetQuickInverse() const
    // {
    //     Mat3x3 quickInverse = *this;
    //     quickInverse.QuickInverse();
    //     return quickInverse;
    // }

    void SetIdentity()
    {
        m[0][0] = 1.f; m[0][1] = 0.f; m[0][2] = 0.f;
        m[1][0] = 0.f; m[1][1] = 1.f; m[1][2] = 0.f;
        m[2][0] = 0.f; m[2][1] = 0.f; m[2][2] = 1.f;
    }


    std::string GetString() const
    {
        std::string mat = 
            "|" + std::to_string((int) m[0][0]) + "  " + std::to_string((int) m[0][1]) + "  " + std::to_string((int) m[0][2]) + "  " + std::to_string((int) m[0][3]) + "|\n" +
            "|" + std::to_string((int) m[1][0]) + "  " + std::to_string((int) m[1][1]) + "  " + std::to_string((int) m[1][2]) + "  " + std::to_string((int) m[1][3]) + "|\n" +
            "|" + std::to_string((int) m[2][0]) + "  " + std::to_string((int) m[2][1]) + "  " + std::to_string((int) m[2][2]) + "  " + std::to_string((int) m[2][3]) + "|\n" 
        ;

        return mat;
    }
};
