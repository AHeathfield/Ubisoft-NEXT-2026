#include "Camera.h"
#include <string>

Camera::Camera()
{
    UpdateProjection();
    UpdateView();
}

Camera::Camera(const float fov_degrees, const float aspect_ratio, const float near, const float far, const Transform3DComponent& transform)
    : mFovDegrees(fov_degrees),
    mAspectRatio(aspect_ratio),
    mNear(near),
    mFar(far),
    mTransform(transform)
{
    UpdateProjection();
    UpdateView();
}


Vector3 Camera::GetLookDirection() const
{
//     Mat4x4 matCameraRotX = Mat4x4::RotationX(-mTransform.GetRotationRAD().y);
//     Mat4x4 matCameraRotY = Mat4x4::RotationY(-mTransform.GetRotationRAD().x);
//     // Mat4x4 matCameraRotZ = Mat4x4::RotationZ(mTransform.GetRotationRAD().x);
//     Mat4x4 matCameraRot = matCameraRotY * matCameraRotX;// * matCameraRotZ;
//     return matCameraRot.GetMultiplyVector3(Vector3::FORWARD);

    // Vector3 rotation = mTransform.GetRotationRAD();
    //
    // Mat4x4 yaw   = Mat4x4::RotationY(rotation.y);
    // Mat4x4 pitch = Mat4x4::RotationX(rotation.x);
    //
    // // Yaw first, then pitch in yawed space
    // Mat4x4 cameraRot = yaw * pitch;
    //
    // return cameraRot.GetMultiplyVector3(Vector3::FORWARD).GetNormal();
    Vector3 rot = mTransform.GetRotationRAD();

    float yaw   = rot.x;
    float pitch = rot.y;

    Vector3 forward;
    forward.x = cosf(pitch) * sinf(yaw);
    forward.y = sinf(pitch);
    forward.z = cosf(pitch) * cosf(yaw);

    return forward.GetNormal();
}

void Camera::UpdateProjection()
{
    float fFovRad = 1.f / std::tanf(mFovDegrees * 0.5f / 180.f * 3.14159f);
    mMatProjection.m[0][0] = mAspectRatio * fFovRad;
    mMatProjection.m[1][1] = fFovRad;
    mMatProjection.m[2][2] = mFar / (mFar - mNear);
    mMatProjection.m[3][2] = (-mFar * mNear) / (mFar - mNear);
    mMatProjection.m[2][3] = 1.0f;
    mMatProjection.m[3][3] = 0.0f;
}

void Camera::UpdateView()
{
    Vector3 target = mTransform.GetPosition() + GetLookDirection();
    // mMatView.SetPointAt(mTransform.GetPosition(), target);
    // mMatView.QuickInverse();

    Mat4x4 matCamera;
    matCamera.SetPointAt(mTransform.GetPosition(), target);
    mMatView = matCamera.GetQuickInverse();
}


float Camera::GetFovDeg() const
{
    return mFovDegrees;
}


float Camera::GetAspectRatio() const
{
    return mAspectRatio;
}


float Camera::GetNear() const
{
    return mNear;
}


float Camera::GetFar() const
{
    return mFar;
}


Mat4x4 Camera::GetProjectionMatrix() const
{
    return mMatProjection;
}


Mat4x4 Camera::GetViewMatrix() const
{
    return mMatView;
}


Transform3DComponent Camera::GetTransform() const
{
    return mTransform;
}


void Camera::SetFovDeg(const float deg)
{
    mFovDegrees = deg;
}


void Camera::SetAspectRatio(const float ratio)
{
    mAspectRatio = ratio;
}


void Camera::SetNear(const float near)
{
    mNear = near;
}


void Camera::SetFar(const float far)
{
    mFar = far;
}


void Camera::SetProjectionMatrix(const Mat4x4& projection)
{
    mMatProjection = projection;
}


void Camera::SetViewMatrix(const Mat4x4& view)
{
    mMatView = view;
}


void Camera::SetTransform(const Transform3DComponent& transform)
{
    mTransform = transform;
}
