#pragma once

#include "../Math/Vector3.h"
#include "../Math/Mat4x4.h"
#include "../Math/Mat3x3.h"

// Currently rotation is euler angles that are converted into matrices, this method is error prone
// Can make the system more stable by changing rotation to Quaternion, but that is a lot of work and changes...
// Rotation for graphics and physics will use radians internally but externally will be degrees
class Transform3DComponent
{
	Vector3 mPosition = Vector3(0, 0, 0);
	Vector3 mRotation = Vector3(0, 0, 0);
	Vector3 mScale = Vector3(1.f, 1.f, 1.f);

public:
	bool wasChanged = true;

public:
	Transform3DComponent() = default;
	Transform3DComponent(const Vector3& position, const Vector3& rotation_deg, const Vector3& scale)
		: mPosition(position),
		mScale(scale)
	{
		mRotation.x = utility::degToRad(rotation_deg.x);
		mRotation.y = utility::degToRad(rotation_deg.y);
		mRotation.z = utility::degToRad(rotation_deg.z);
	}

	Vector3 GetPosition() const
	{
		return mPosition;
	}
	void SetPosition(const Vector3& position)
	{
		mPosition = position;
		wasChanged = true;
	}

	Vector3 GetRotationDEG() const
	{
		return Vector3(utility::radToDeg(mRotation.x), utility::radToDeg(mRotation.y), utility::radToDeg(mRotation.z));
	}
	void SetRotationDEG(const Vector3& rotation_deg)
	{
		mRotation.x = utility::degToRad(rotation_deg.x);
		mRotation.y = utility::degToRad(rotation_deg.y);
		mRotation.z = utility::degToRad(rotation_deg.z);
		wasChanged = true;
	}
	Vector3 GetRotationRAD() const
	{
		return mRotation;
	}
	void SetRotationRAD(const Vector3& rotation_rad)
	{
		mRotation = rotation_rad;
		wasChanged = true;
	}

	Vector3 GetScale() const
	{
		return mScale;
	}
	void SetScale(const Vector3& scale)
	{
		mScale = scale;
		wasChanged = true;
	}



	// TODO: Change name to GetTransform (I think it makes more sense idk)
	// Common order to get Transform Matrix: mScale * mRotation * translation
	Mat4x4 GetWorldMatrix() const
	{
		Mat4x4 matScale = Mat4x4::Scale(mScale);
		// matScale.SetScale(mScale.x, mScale.y, mScale.z);

		// Mat4x4 matRotX, matRotY, matRotZ;
		// matRotX.SetRotationX(mRotation.x);
		// matRotY.SetRotationY(mRotation.y);
		// matRotZ.SetRotationZ(mRotation.z);
		Mat4x4 matRot = Mat4x4::Rotation(mRotation);

		Mat4x4 matTranslation = Mat4x4::Translation(mPosition);
		// matTranslation.SetTranslation(mPosition.x, mPosition.y, mPosition.z);

		Mat4x4 matWorld;
		matWorld = matScale;
		// matWorld *= matRotY * matRotZ * matRotX;
		matWorld *= matRot;
		matWorld *= matTranslation;

		return matWorld;
	}

	
	Mat3x3 GetRotationMatrix() const
	{
		// Mat4x4 matRotX = Mat4x4::RotationX(mRotation.x);
		// Mat4x4 matRotY = Mat4x4::RotationY(mRotation.y);
		// Mat4x4 matRotZ = Mat4x4::RotationZ(mRotation.z);
		// matRotY.SetRotationY(mRotation.y);
		// matRotZ.SetRotationZ(mRotation.z);

		// return (matRotY * matRotZ * matRotX).GetMat3x3();
		return Mat4x4::Rotation(mRotation).GetMat3x3();
	}
};
