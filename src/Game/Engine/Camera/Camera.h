#pragma once

#include "../Math/Vector3.h"
#include "../Math/Mat4x4.h"
#include "../Components/Transform3DComponent.h"
#include "../../../ContestAPI/AppSettings.h"

// This is standalone not part of an entity 
class Camera
{
public:
    Camera();
    Camera(const float fov_degrees, const float aspect_ratio, const float near, const float far, const Transform3DComponent& transform);


    void UpdateProjection();
    void UpdateView();

    float GetFovDeg() const;
    float GetAspectRatio() const;
    float GetNear() const;
    float GetFar() const;
    Mat4x4 GetProjectionMatrix() const;
    Mat4x4 GetViewMatrix() const;
    Vector3 GetLookDirection() const;
    Transform3DComponent GetTransform() const;

    void SetFovDeg(const float deg); 
    void SetAspectRatio(const float ratio);
    void SetNear(const float near);
    void SetFar(const float far);
    void SetProjectionMatrix(const Mat4x4& projection);
    void SetViewMatrix(const Mat4x4& view);
    void SetTransform(const Transform3DComponent& transform);

private:
    float mFovDegrees = 90.f;
    float mAspectRatio = (float) APP_VIRTUAL_HEIGHT / (float) APP_VIRTUAL_WIDTH;
    float mNear = 0.1f;
    float mFar = 1000.f;
    Mat4x4 mMatProjection = Mat4x4();
    Mat4x4 mMatView = Mat4x4();
    Transform3DComponent mTransform = Transform3DComponent();


    // void MoveForward(const float speed)
    // {
    //     position = position + (GetLookDirection() * speed);
    // }
    //
    // void MoveBackward(const float speed)
    // {
    //     position = position - (GetLookDirection() * speed);
    // }
    //
    // void MoveRight(const float speed)
    // {
    //     // The cross of the direction facing and the world UP should be the right dir relative to the facing dir
    //     Vector3 local_right = GetLookDirection().GetCrossProduct(Vector3::UP);
    //     position = position + (local_right * speed);
    // }
    //
    // void MoveLeft(const float speed)
    // {
    //     // The cross of the direction facing and the world DOWN should be the left dir relative to the facing dir
    //     Vector3 local_left = GetLookDirection().GetCrossProduct(Vector3::DOWN);
    //     position = position + (local_left * speed);
    // }
};
