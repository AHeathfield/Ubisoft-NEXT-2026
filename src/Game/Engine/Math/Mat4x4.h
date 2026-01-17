#pragma once

#include "Vector3.h"
#include "Mat3x3.h"
#include <string>

// Followed this youtube series by javidx9 https://www.youtube.com/watch?v=ih20l3pJoeU
// This is a quick refresher on some of the methods https://www.codinglabs.net/article_world_view_projection_matrix.aspx
struct Mat4x4
{
    // [row][col]
    float m[4][4] = { 0 };

    static Mat4x4 Identity()
    {
        Mat4x4 result;
        result.m[0][0] = 1.f;
        result.m[1][1] = 1.f;
        result.m[2][2] = 1.f;
        result.m[3][3] = 1.f;
        return result;
    }

    static Mat4x4 RotationX(float pitch_rad)
    {
        Mat4x4 result;
        result.m[0][0] = 1.f;
        result.m[1][1] = std::cosf(pitch_rad);
        result.m[1][2] = std::sinf(pitch_rad);
        result.m[2][1] = -1 * std::sinf(pitch_rad);
        result.m[2][2] = std::cosf(pitch_rad);
        result.m[3][3] = 1.f;
        return result;
    }

    static Mat4x4 RotationY(float yaw_rad)
    {
        Mat4x4 result;
        result.m[0][0] = std::cosf(yaw_rad);
        result.m[0][2] = -1 * std::sinf(yaw_rad);
        result.m[2][0] = std::sinf(yaw_rad);
        result.m[1][1] = 1.f;
        result.m[2][2] = std::cosf(yaw_rad);
        result.m[3][3] = 1.f;
        return result;
    }

    static Mat4x4 RotationZ(float roll_rad)
    {
        Mat4x4 result;
        result.m[0][0] = std::cosf(roll_rad);
        result.m[0][1] = std::sinf(roll_rad);
        result.m[1][0] = -1 * std::sinf(roll_rad);
        result.m[1][1] = std::cosf(roll_rad);
        result.m[2][2] = 1.f;
        result.m[3][3] = 1.f;
        return result;
    }

    static Mat4x4 Rotation(const Vector3& rotation_rad)
    {
        Mat4x4 matRotX = Mat4x4::RotationX(rotation_rad.x);
        Mat4x4 matRotY = Mat4x4::RotationY(rotation_rad.y);
        Mat4x4 matRotZ = Mat4x4::RotationZ(rotation_rad.z);

        return matRotZ * matRotX * matRotY;
    }

    static Mat4x4 Translation(float x, float y, float z)
    {
        Mat4x4 result;
        result.m[0][0] = 1.f;
        result.m[1][1] = 1.f;
        result.m[2][2] = 1.f;
        result.m[3][3] = 1.f;
        result.m[3][0] = x;
        result.m[3][1] = y;
        result.m[3][2] = z;
        return result;
    }

    static Mat4x4 Translation(const Vector3& translation)
    {
        return Mat4x4::Translation(translation.x, translation.y, translation.z);
    }

    static Mat4x4 Scale(float x, float y, float z)
    {
        Mat4x4 result;
        result.m[0][0] = x;
        result.m[1][1] = y;
        result.m[2][2] = z;
        result.m[3][3] = 1;
        return result;
    }

    static Mat4x4 Scale(const Vector3& scale)
    {
        return Mat4x4::Scale(scale.x, scale.y, scale.z);
    }

    static Mat4x4 Projection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
    {
        Mat4x4 result;
        float fFovRad = 1.f / std::tanf(fFovDegrees * 0.5f / 180.f * 3.14159f);
        result.m[0][0] = fAspectRatio * fFovRad;
        result.m[1][1] = fFovRad;
        result.m[2][2] = fFar / (fFar - fNear);
        result.m[3][2] = (-fFar * fNear) / (fFar - fNear);
        result.m[2][3] = 1.0f;
        result.m[3][3] = 0.0f;
        return result;
    }



    // Operator overloads
    Mat4x4 operator*(const Mat4x4& other)
    {
        Mat4x4 matrix;
        for (int c = 0; c < 4; c++)
        {
            for (int r = 0; r < 4; r++)
            {
                matrix.m[r][c] = m[r][0] * other.m[0][c] + m[r][1] * other.m[1][c] + m[r][2] * other.m[2][c] + m[r][3] * other.m[3][c];
            }
        }
        return matrix;
    }
    Mat4x4& operator*=(const Mat4x4& other)
    {
        // Just reusing the operator above
        *this = *this * other;
        return *this;
    }

    std::array<float, 16> GetFloatArray() const
    {
        return std::array<float, 16> {
            m[0][0], m[0][1], m[0][2], m[0][3],
            m[1][0], m[1][1], m[1][2], m[1][3],
            m[2][0], m[2][1], m[2][2], m[2][3],
            m[3][0], m[3][1], m[3][2], m[3][3]
        };
    }


    Mat3x3 GetMat3x3()
    {
        Mat3x3 mat;
        mat.m[0][0] = m[0][0];
        mat.m[0][1] = m[0][1];
        mat.m[0][2] = m[0][2];
        mat.m[1][0] = m[1][0];
        mat.m[1][1] = m[1][1];
        mat.m[1][2] = m[1][2];
        mat.m[2][0] = m[2][0];
        mat.m[2][1] = m[2][1];
        mat.m[2][2] = m[2][2];

        return mat;
    }


    Vector3 GetMultiplyVector3(const Vector3& i) const
    {
        Vector3 v;
        v.x = i.x * m[0][0] + i.y * m[1][0] + i.z * m[2][0] + i.w * m[3][0];
        v.y = i.x * m[0][1] + i.y * m[1][1] + i.z * m[2][1] + i.w * m[3][1];
        v.z = i.x * m[0][2] + i.y * m[1][2] + i.z * m[2][2] + i.w * m[3][2];
        v.w = i.x * m[0][3] + i.y * m[1][3] + i.z * m[2][3] + i.w * m[3][3];
        return v;
    }

    // Mat4x4 GetQuickInverse() const
    // {
    //     Mat4x4 quickInverse = *this;
    //     quickInverse.QuickInverse();
    //     return quickInverse;
    // }

    void SetIdentity()
    {
        m[0][0] = 1.f; m[0][1] = 0.f; m[0][2] = 0.f; m[0][3] = 0.f;
        m[1][0] = 0.f; m[1][1] = 1.f; m[1][2] = 0.f; m[1][3] = 0.f;
        m[2][0] = 0.f; m[2][1] = 0.f; m[2][2] = 1.f; m[2][3] = 0.f;
        m[3][0] = 0.f; m[3][1] = 0.f; m[3][2] = 0.f; m[3][3] = 1.f;
    }

    // Replaced with statics 
    // void SetRotationX(float angleRad)
    // {
    //     m[0][0] = 1.f;
    //     m[1][1] = std::cosf(angleRad);
    //     m[1][2] = std::sinf(angleRad);
    //     m[2][1] = -1 * std::sinf(angleRad);
    //     m[2][2] = std::cosf(angleRad);
    //     m[3][3] = 1.f;
    // }
    //
    // void SetRotationY(float angleRad)
    // {
    //     // m[0][0] = std::cosf(angleRad);
    //     // m[0][2] = std::sinf(angleRad);
    //     // m[2][0] = -1 * std::sinf(angleRad);
    //     // m[1][1] = 1.f;
    //     // m[2][2] = std::cosf(angleRad);
    //     // m[3][3] = 1.f;
    //     m[0][0] = std::cosf(angleRad);
    //     m[0][2] = std::sinf(angleRad);
    //     m[2][0] = -1 * std::sinf(angleRad);
    //     m[1][1] = 1.f;
    //     m[2][2] = std::cosf(angleRad);
    //     m[3][3] = 1.f;
    // }
    //
    // void SetRotationZ(float angleRad)
    // {
    //     m[0][0] = std::cosf(angleRad);
    //     m[0][1] = std::sinf(angleRad);
    //     m[1][0] = -1 * std::sinf(angleRad);
    //     m[1][1] = std::cosf(angleRad);
    //     m[2][2] = 1.f;
    //     m[3][3] = 1.f;
    // }
    //
    // void SetTranslation(float x, float y, float z)
    // {
    //     m[0][0] = 1.f;
    //     m[1][1] = 1.f;
    //     m[2][2] = 1.f;
    //     m[3][3] = 1.f;
    //     m[3][0] = x;
    //     m[3][1] = y;
    //     m[3][2] = z;
    // }
    //
    // void SetScale(float x, float y, float z)
    // {
    //     m[0][0] = x;
    //     m[1][1] = y;
    //     m[2][2] = z;
    //     m[3][3] = 1;
    // }
    //
    // void SetProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
    // {
    //     float fFovRad = 1.f / std::tanf(fFovDegrees * 0.5f / 180.f * 3.14159f);
    //     m[0][0] = fAspectRatio * fFovRad;
    //     m[1][1] = fFovRad;
    //     m[2][2] = fFar / (fFar - fNear);
    //     m[3][2] = (-fFar * fNear) / (fFar - fNear);
    //     m[2][3] = 1.0f;
    //     m[3][3] = 0.0f;
    // }

    // pos = where obj should be
    // target = forward vector
    // up = up vector
    void SetPointAt(const Vector3& pos, const Vector3& target)
    {
        // Calculate new forward direction
        Vector3 newForward = target - pos;
        newForward.Normalize();

        // Calculate new Up direction (scalar multiplication)
        Vector3 a = newForward * Vector3::UP.GetDotProduct(newForward);
        Vector3 newUp = Vector3::UP - a;
        newUp.Normalize();

        // New Right direction is easy, its just cross product
        Vector3 newRight = newUp.GetCrossProduct(newForward);

        // Construct Dimensioning and Translation Matrix
        m[0][0] = newRight.x;	m[0][1] = newRight.y;	m[0][2] = newRight.z;	m[0][3] = 0.0f;
        m[1][0] = newUp.x;		m[1][1] = newUp.y;		m[1][2] = newUp.z;		m[1][3] = 0.0f;
        m[2][0] = newForward.x;	m[2][1] = newForward.y;	m[2][2] = newForward.z;	m[2][3] = 0.0f;
        m[3][0] = pos.x;		m[3][1] = pos.y;		m[3][2] = pos.z;		m[3][3] = 1.0f;
    }

    // IMPORTANT: ONLY WORKS FOR Rotation/Translation Matrices!!!
    void QuickInverse()
    {
        m[0][0] = m[0][0]; m[0][1] = m[1][0]; m[0][2] = m[2][0]; m[0][3] = 0.0f;
        m[1][0] = m[0][1]; m[1][1] = m[1][1]; m[1][2] = m[2][1]; m[1][3] = 0.0f;
        m[2][0] = m[0][2]; m[2][1] = m[1][2]; m[2][2] = m[2][2]; m[2][3] = 0.0f;
        m[3][0] = -(m[3][0] * m[0][0] + m[3][1] * m[1][0] + m[3][2] * m[2][0]);
        m[3][1] = -(m[3][0] * m[0][1] + m[3][1] * m[1][1] + m[3][2] * m[2][1]);
        m[3][2] = -(m[3][0] * m[0][2] + m[3][1] * m[1][2] + m[3][2] * m[2][2]);
        m[3][3] = 1.0f;
    }

    Mat4x4 GetQuickInverse()
    {
        Mat4x4 matrix;
        matrix.m[0][0] = m[0][0]; matrix.m[0][1] = m[1][0]; matrix.m[0][2] = m[2][0]; matrix.m[0][3] = 0.0f;
        matrix.m[1][0] = m[0][1]; matrix.m[1][1] = m[1][1]; matrix.m[1][2] = m[2][1]; matrix.m[1][3] = 0.0f;
        matrix.m[2][0] = m[0][2]; matrix.m[2][1] = m[1][2]; matrix.m[2][2] = m[2][2]; matrix.m[2][3] = 0.0f;
        matrix.m[3][0] = -(m[3][0] * matrix.m[0][0] + m[3][1] * matrix.m[1][0] + m[3][2] * matrix.m[2][0]);
        matrix.m[3][1] = -(m[3][0] * matrix.m[0][1] + m[3][1] * matrix.m[1][1] + m[3][2] * matrix.m[2][1]);
        matrix.m[3][2] = -(m[3][0] * matrix.m[0][2] + m[3][1] * matrix.m[1][2] + m[3][2] * matrix.m[2][2]);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    Mat4x4 GetInverse() const
    {
        // In expanded form to avoid overhead of loop and function calls
        float det 
		= m[0][0] * m[1][1] * m[2][2] * m[3][3] + m[0][0] * m[1][2] * m[2][3] * m[3][1] + m[0][0] * m[1][3] * m[2][1] * m[3][2]
		+ m[0][1] * m[1][0] * m[2][3] * m[3][2] + m[0][1] * m[1][2] * m[2][0] * m[3][3] + m[0][1] * m[1][3] * m[2][2] * m[3][0]
		+ m[0][2] * m[1][0] * m[2][1] * m[3][3] + m[0][2] * m[1][1] * m[2][3] * m[3][0] + m[0][2] * m[1][3] * m[2][0] * m[3][1]
		+ m[0][3] * m[1][0] * m[2][2] * m[3][1] + m[0][3] * m[1][1] * m[2][0] * m[3][2] + m[0][3] * m[1][2] * m[2][1] * m[3][0]
		- m[0][0] * m[1][1] * m[2][3] * m[3][2] - m[0][0] * m[1][2] * m[2][1] * m[3][3] - m[0][0] * m[1][3] * m[2][2] * m[3][1]
		- m[0][1] * m[1][0] * m[2][2] * m[3][3] - m[0][1] * m[1][2] * m[2][3] * m[3][0] - m[0][1] * m[1][3] * m[2][0] * m[3][2]
		- m[0][2] * m[1][0] * m[2][3] * m[3][1] - m[0][2] * m[1][1] * m[2][0] * m[3][3] - m[0][2] * m[1][3] * m[2][1] * m[3][0]
		- m[0][3] * m[1][0] * m[2][1] * m[3][2] - m[0][3] * m[1][1] * m[2][2] * m[3][0] - m[0][3] * m[1][2] * m[2][0] * m[3][1];

	if (CMP_FLOAT(det, 0.0f))
		return Mat4x4::Identity(); 

	float i_det = 1.0f / det;

	Mat4x4 result;
	result.m[0][0] = (m[1][1] * m[2][2] * m[3][3] + m[1][2] * m[2][3] * m[3][1] + m[1][3] * m[2][1] * m[3][2] - m[1][1] * m[2][3] * m[3][2] - m[1][2] * m[2][1] * m[3][3] - m[1][3] * m[2][2] * m[3][1]) * i_det;
	result.m[0][1] = (m[0][1] * m[2][3] * m[3][2] + m[0][2] * m[2][1] * m[3][3] + m[0][3] * m[2][2] * m[3][1] - m[0][1] * m[2][2] * m[3][3] - m[0][2] * m[2][3] * m[3][1] - m[0][3] * m[2][1] * m[3][2]) * i_det;
	result.m[0][2] = (m[0][1] * m[1][2] * m[3][3] + m[0][2] * m[1][3] * m[3][1] + m[0][3] * m[1][1] * m[3][2] - m[0][1] * m[1][3] * m[3][2] - m[0][2] * m[1][1] * m[3][3] - m[0][3] * m[1][2] * m[3][1]) * i_det;
	result.m[0][3] = (m[0][1] * m[1][3] * m[2][2] + m[0][2] * m[1][1] * m[2][3] + m[0][3] * m[1][2] * m[2][1] - m[0][1] * m[1][2] * m[2][3] - m[0][2] * m[1][3] * m[2][1] - m[0][3] * m[1][1] * m[2][2]) * i_det;
	result.m[1][0] = (m[1][0] * m[2][3] * m[3][2] + m[1][2] * m[2][0] * m[3][3] + m[1][3] * m[2][2] * m[3][0] - m[1][0] * m[2][2] * m[3][3] - m[1][2] * m[2][3] * m[3][0] - m[1][3] * m[2][0] * m[3][2]) * i_det;
	result.m[1][1] = (m[0][0] * m[2][2] * m[3][3] + m[0][2] * m[2][3] * m[3][0] + m[0][3] * m[2][0] * m[3][2] - m[0][0] * m[2][3] * m[3][2] - m[0][2] * m[2][0] * m[3][3] - m[0][3] * m[2][2] * m[3][0]) * i_det;
	result.m[1][2] = (m[0][0] * m[1][3] * m[3][2] + m[0][2] * m[1][0] * m[3][3] + m[0][3] * m[1][2] * m[3][0] - m[0][0] * m[1][2] * m[3][3] - m[0][2] * m[1][3] * m[3][0] - m[0][3] * m[1][0] * m[3][2]) * i_det;
	result.m[1][3] = (m[0][0] * m[1][2] * m[2][3] + m[0][2] * m[1][3] * m[2][0] + m[0][3] * m[1][0] * m[2][2] - m[0][0] * m[1][3] * m[2][2] - m[0][2] * m[1][0] * m[2][3] - m[0][3] * m[1][2] * m[2][0]) * i_det;
	result.m[2][0] = (m[1][0] * m[2][1] * m[3][3] + m[1][1] * m[2][3] * m[3][0] + m[1][3] * m[2][0] * m[3][1] - m[1][0] * m[2][3] * m[3][1] - m[1][1] * m[2][0] * m[3][3] - m[1][3] * m[2][1] * m[3][0]) * i_det;
	result.m[2][1] = (m[0][0] * m[2][3] * m[3][1] + m[0][1] * m[2][0] * m[3][3] + m[0][3] * m[2][1] * m[3][0] - m[0][0] * m[2][1] * m[3][3] - m[0][1] * m[2][3] * m[3][0] - m[0][3] * m[2][0] * m[3][1]) * i_det;
	result.m[2][2] = (m[0][0] * m[1][1] * m[3][3] + m[0][1] * m[1][3] * m[3][0] + m[0][3] * m[1][0] * m[3][1] - m[0][0] * m[1][3] * m[3][1] - m[0][1] * m[1][0] * m[3][3] - m[0][3] * m[1][1] * m[3][0]) * i_det;
	result.m[2][3] = (m[0][0] * m[1][3] * m[2][1] + m[0][1] * m[1][0] * m[2][3] + m[0][3] * m[1][1] * m[2][0] - m[0][0] * m[1][1] * m[2][3] - m[0][1] * m[1][3] * m[2][0] - m[0][3] * m[1][0] * m[2][1]) * i_det;
	result.m[3][0] = (m[1][0] * m[2][2] * m[3][1] + m[1][1] * m[2][0] * m[3][2] + m[1][2] * m[2][1] * m[3][0] - m[1][0] * m[2][1] * m[3][2] - m[1][1] * m[2][2] * m[3][0] - m[1][2] * m[2][0] * m[3][1]) * i_det;
	result.m[3][1] = (m[0][0] * m[2][1] * m[3][2] + m[0][1] * m[2][2] * m[3][0] + m[0][2] * m[2][0] * m[3][1] - m[0][0] * m[2][2] * m[3][1] - m[0][1] * m[2][0] * m[3][2] - m[0][2] * m[2][1] * m[3][0]) * i_det;
	result.m[3][2] = (m[0][0] * m[1][2] * m[3][1] + m[0][1] * m[1][0] * m[3][2] + m[0][2] * m[1][1] * m[3][0] - m[0][0] * m[1][1] * m[3][2] - m[0][1] * m[1][2] * m[3][0] - m[0][2] * m[1][0] * m[3][1]) * i_det;
	result.m[3][3] = (m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1] - m[0][0] * m[1][2] * m[2][1] - m[0][1] * m[1][0] * m[2][2] - m[0][2] * m[1][1] * m[2][0]) * i_det;

	return result;
    }

    std::string GetString() const
    {
        std::string mat = 
            "\n|" + std::to_string(m[0][0]) + "  " + std::to_string(m[0][1]) + "  " + std::to_string(m[0][2]) + "  " + std::to_string(m[0][3]) + "|\n" +
            "|" + std::to_string(m[1][0]) + "  " + std::to_string(m[1][1]) + "  " + std::to_string(m[1][2]) + "  " + std::to_string(m[1][3]) + "|\n" +
            "|" + std::to_string(m[2][0]) + "  " + std::to_string(m[2][1]) + "  " + std::to_string(m[2][2]) + "  " + std::to_string(m[2][3]) + "|\n" +
            "|" + std::to_string(m[3][0]) + "  " + std::to_string(m[3][1]) + "  " + std::to_string(m[3][2]) + "  " + std::to_string(m[3][3]) + "|"
        ;

        return mat;
    }
};
