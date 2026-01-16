#pragma once

#include "../Math/Vector3.h"
#include "../Math/Mat4x4.h"

struct Camera
{
    Vector3 position = Vector3(0, 0, 0);
    Vector3 rotation = Vector3(0, 0, 0);

    Vector3 GetLookDirection()
    {
        // This is really weird but it works lol
        Mat4x4 matCameraRotX = Mat4x4::RotationX(-rotation.y);
        Mat4x4 matCameraRotY = Mat4x4::RotationY(-rotation.x);
        Mat4x4 matCameraRotZ = Mat4x4::RotationZ(rotation.z);
        Mat4x4 matCameraRot = matCameraRotY * matCameraRotX * matCameraRotZ;
        return matCameraRot.GetMultiplyVector3(Vector3::FORWARD);
    }

    void MoveForward(const float speed)
    {
        position = position + (GetLookDirection() * speed);
    }

    void MoveBackward(const float speed)
    {
        position = position - (GetLookDirection() * speed);
    }

    void MoveRight(const float speed)
    {
        // The cross of the direction facing and the world UP should be the right dir relative to the facing dir
        Vector3 local_right = GetLookDirection().GetCrossProduct(Vector3::UP);
        position = position + (local_right * speed);
    }

    void MoveLeft(const float speed)
    {
        // The cross of the direction facing and the world DOWN should be the left dir relative to the facing dir
        Vector3 local_left = GetLookDirection().GetCrossProduct(Vector3::DOWN);
        position = position + (local_left * speed);
    }
};
