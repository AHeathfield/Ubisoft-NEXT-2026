#pragma once

#include "../Core/ECS.h"
#include "../Math/Vector3.h"
#include "../Components/RigidBodyComponent.h"
#include "../Physics/CollisionManifold.h"
#include <unordered_set>

// Handles rigid bodies
// This system uses linear projection but more modern engines use an iterative physics solver, see https://www.gdcvault.com/play/1020603/Physics-for-Game-Programmers-Understanding
class RigidBodySystem : public System
{
public:
    // Apply Forces -> Apply impulses (collision response) -> Integrate Velocity and Positions -> Adjustments
    void Update(float deltaTime) override;
    void OnEntityAdded(Entity entity) override;
    void OnEntityRemoved(Entity entity) override;

    Vector3 GetGravity() const;
    void SetGravity(const Vector3& gravity);
    float GetDamping() const;
    void SetDamping(float damping);

    // TODO: Add getter and setters for other privates and create a clamp method in utility!!

private:
    struct ImpulseData
    {
        Entity entA;
        Entity entB;
        CollisionManifold collision;
    };
    void applyForces(RigidBodyComponent& rb);
    void applyImpulse(const ImpulseData& impData, int c);
    void addRotationalImpulse(Entity ent, const Vector3& point, const Vector3& impulse);

    // This is the ApplyImpulse from TB pre much
    void physicsResponse(Entity self, Entity other, const CollisionManifold& collision);

    bool shouldSleep(RigidBodyComponent& rb) const;


private:

    std::unordered_set<Entity> mDeleted;
    std::vector<ImpulseData> mImpulses;
    std::unordered_set<Entity> mCollisions;
    
    // float mRestVelocity = 1.0f;

    Vector3 mGravity = Vector3(0, 9.82f, 0); // + y is down ;)
    float mDamping = 0.98f; // Mainly used to simulate air friction

    // Indicates how much positional correction to apply (smaller allows more penetration)
    // Ideal values are between 0.2 and 0.8, so the set method will clamp this value
    float mLinearProjectionPercent = 0.45f;

    // Determines how much to allow objects to penetrate.
    // Helps avoid jitter (larger means less jitter) (lower means more flat)
    // Ideal values between 0.01 and 0.1 will be clamped via SetMethod
    float mPenetrationSlack = 0.01f;

    // Even though physics solver isn't iterative, we do solve physics in several steps
    // More iterations = more accurate physics
    // Ideal values between 1 and 20, TB says 6-8 works well
    int mImpulseIteration = 5;
};

