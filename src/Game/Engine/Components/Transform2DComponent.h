#pragma once

#include "../Math/Vector2.h"
#include "../Core/Utility.h"

class Transform2DComponent
{
    Vector2 mPosition = Vector2(0, 0);
    float mRotation = 0.0f; // RAD
    Vector2 mScale = Vector2(1.f, 1.f);
    bool mChanged = true;

public:
    Transform2DComponent() = default;
    Transform2DComponent(const Vector2& position, const float rotation_deg, const Vector2& scale)
        : mPosition(position),
        mScale(scale)
    {
        mRotation = utility::degToRad(rotation_deg);
    }

    Vector2 GetPosition() const
    {
        return mPosition;
    }
    void SetPosition(const Vector2& position)
    {
	    mPosition = position;
	    mChanged = true;
    }

    float GetRotationDEG() const
    {
        return utility::radToDeg(mRotation);
    }
    void SetRotationDEG(const float rotation_deg)
    {
        mRotation = utility::degToRad(rotation_deg);
        mChanged = true;
    }
    float GetRotationRAD() const
    {
        return mRotation;
    }
    void SetRotationRAD(const float rotation_rad)
    {
        mRotation = rotation_rad;
        mChanged = true;
    }

    Vector2 GetScale() const
    {
        return mScale;
    }
    void SetScale(const Vector2& scale)
    {
        mScale = scale;
        mChanged = true;
    }

    bool IsChanged() const
    {
        return mChanged;
    }
    void SetChanged(const bool change)
    {
        mChanged = change;
    }
};
