#pragma once

#include "../Math/Vector3.h"
#include "../Math/ShapeType.h"
#include "../Math/Mat4x4.h"
#include "../Core/Timer.h"
#include "ColliderComponent.h"
#include <string>

// NOTE: My rigidbody is RigidBodyVolume in TB

// A Rigidbody is an object that does not change its shape, its rigid.
// If you dropped a ball thats rigid when it hits the ground the shape of the ball won't change
struct RigidbodyComponent
{
    bool isStatic = false;
    bool isAwake = true;
    Timer sleepTimer = Timer(true, 0.5f); // Stays asleep for this long then wakes up
    Timer awakeTimer = Timer(false, 1.0f); // Stays awake for this long before testing if it should sleep
    float sleepVelocityThreshold = 1.0f;
    float sleepAngularVelocityThreshold = 1.0f;

    // No pos bc transform has that
    Vector3 velocity = Vector3(0, 0, 0);
    Vector3 angularVelocity = Vector3(0, 0, 0);
    // Vector3 orientation = Vector3(0, 0, 0); // HANDLED by rotation in Transform3DComponent I THINK...
    // Prob add angular vel here too...
    Vector3 forces = Vector3(0, 0, 0); // Sum of all forces
    Vector3 torques = Vector3(0, 0, 0); // Sum of torques
    
    float mass = 1.f;
    float restitution = 0.5f; // Coefficient of restitution
    float friction = 0.6f;


    // Force effects velocity over time, impulse has a direct and immediate effect on velocity
    void AddLinearImpulse(const Vector3 impulse)
    {
        velocity = velocity + impulse;
    }

    float GetInvMass() const
    {
        if (mass == 0.f)
            return 0.f;

        return 1.f / mass;
    }

    // Currently only for sphere, OBB, (and AABB, maybe)
    // I could make it rigidbody has its own shape, but I think just making it the same as the collider makes sense for most situations
    Mat4x4 GetInvTensor(const ColliderComponent& colliderComp)
    {
        // vars to be used for main diagonal of the matrix
        float ix = 0.f;
        float iy = 0.f;
        float iz = 0.f;
        float iw = 0.f;

        // See Cookbook for how this equation works
        if (!isStatic)
        {
            if (colliderComp.shape == ShapeType::SPHERE_3D)
            {
                float r2 = colliderComp.collider.sphere.radius * colliderComp.collider.sphere.radius;
                float fraction = (2.f / 5.f);
                ix = r2 * mass * fraction;
                iy = r2 * mass * fraction;
                iy = r2 * mass * fraction;
                iw = 1.f;
            }
            else if (colliderComp.shape == ShapeType::OBB_3D)
            {
                Vector3 size = colliderComp.collider.obb.size * 2.f;
                float fraction = (1.f / 12.f);
                float x2 = size.x * size.x;
                float y2 = size.y * size.y;
                float z2 = size.z * size.z;
                ix = (y2 + z2) * mass * fraction;
                ix = (x2 + z2) * mass * fraction;
                ix = (x2 + y2) * mass * fraction;
                iw = 1.f;
            }
            else if (colliderComp.shape == ShapeType::AABB_3D)
            {
                Vector3 size = colliderComp.collider.aabb.size * 2.f;
                float fraction = (1.f / 12.f);
                float x2 = size.x * size.x;
                float y2 = size.y * size.y;
                float z2 = size.z * size.z;
                ix = (y2 + z2) * mass * fraction;
                ix = (x2 + z2) * mass * fraction;
                ix = (x2 + y2) * mass * fraction;
                iw = 1.f;
            }
        }

        Mat4x4 result;
        result.m[0][0] = ix;
        result.m[1][1] = iy;
        result.m[2][2] = iz;
        result.m[3][3] = iw;

        return result.GetInverse();
    }

    std::string GetString() const
    {
        std::string result = "Vel = " + velocity.GetString() + "\n" + "AngVel = " + angularVelocity.GetString() + "\n" + "forces = " + forces.GetString() + "\n" + "torques = " + torques.GetString() + "\n" + "mass = " + std::to_string(mass) + "\n" + "restitution = " + std::to_string(restitution) + "\n" + "friction = " + std::to_string(friction) + "\n";

	return result;
    }


};
