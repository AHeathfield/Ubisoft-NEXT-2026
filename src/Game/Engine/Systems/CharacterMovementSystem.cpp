#include "CharacterMovementSystem.h"
#include "CollisionSystem.h"
#include "../Core/GameManager.h"
#include "../Components/ColliderComponent.h"
#include "../Components/CharacterBodyComponent.h"
#include "../Components/RigidBodyComponent.h"
#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>


void CharacterMovementSystem::Update(float deltaTime)
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();
    auto collisionSystem = coordinator.GetSystem<CollisionSystem>();

    if (!collisionSystem->IsInit())
        return;

    for (const auto& entity : mEntities)
    {
        auto& transform = coordinator.GetComponent<Transform3DComponent>(entity);
        auto& character = coordinator.GetComponent<CharacterBodyComponent>(entity);
        const auto& collider = coordinator.GetComponent<ColliderComponent>(entity);

        character.isOnCeiling = false;
        character.isOnFloor = false;
        character.isOnWall = false;

        // Remainin
        Vector3 remainingMove = character.velocity * deltaTime;
        Vector3 position = transform.GetPosition();

        // Idea is to do a raycast test, but expand the collider being tested by the size (scale) of the characters collider
        // So we can do a raycast from the transform position and not like the edge of the character collider
        ColliderComponent expandedCollider;
        Transform3DComponent expandedTransform;
        Ray3 ray;
        RaycastResult outResult, testResult;

        // TESTING
        // position += remainingMove;

        // Slides
        for (int i = 0; i < character.maxSlides; i++)
        {
            if (remainingMove.GetMagnitudeSquared() < _EPS)
                break;

            outResult.Reset();
            outResult.t = FLT_MAX;
            ray = Ray3(position, remainingMove.GetNormal());
            std::unordered_set<Entity> ents_to_check = collisionSystem->QueryStaticRay(ray, remainingMove.GetMagnitude());

            // Getting the closest raycast result
            for (const auto& entityB : ents_to_check)
            {
                if (entity == entityB)
                    continue;

                if (!coordinator.HasComponent<RigidBodyComponent>(entityB))
                    continue;

                testResult.Reset();

                // Creating the expanded collider
                const auto& transformB = coordinator.GetComponent<Transform3DComponent>(entityB);
                const auto& collB = coordinator.GetComponent<ColliderComponent>(entityB);
                expandedTransform = transformB;
                expandedTransform.SetScale(transformB.GetScale() + transform.GetScale());
                expandedCollider = collB;
                expandedCollider.UpdateWorldSpace(expandedTransform);

                collisionSystem->RaycastCollider(ray, testResult, expandedCollider);

                if (testResult.hit && testResult.t < outResult.t)
                {
                    outResult = testResult;
                }
            }


            if (!outResult.hit)
            {
                position += remainingMove;
                break;
            }

            // If they just jumped and its a floor collision, ignore it
            if (character.justJumped && outResult.normal.GetDotProduct(character.upDirection) > character.floorThreshold)
            {
                continue;
            }

            // Ensuring normal opposes dir
            if (outResult.normal.GetDotProduct(ray.direction) > 0)
                outResult.normal = outResult.normal * -1.f;

            // Moving right before the impact
            float moveDist = outResult.t - character.safeMargin;
            if (moveDist > 0.f)
                position += ray.direction * moveDist;
            // float moveDist = outResult.t - character.safeMargin;

            // Remove motion into surface
            remainingMove -= outResult.normal * remainingMove.GetDotProduct(outResult.normal);


            // State handling
            float upDot = outResult.normal.GetDotProduct(character.upDirection);
            if (upDot > character.floorThreshold)
            {
                // The snap method might miss edge collider detection
                character.isOnFloor = true;
            }
            else if (upDot < -character.floorThreshold)
            {
                character.isOnCeiling = true;
            }
            else
            {
                character.isOnWall = true;
            }


            if (remainingMove.GetMagnitudeSquared() < character.safeMargin)
            {
                break;
            }
        }








        // transform.SetPosition(position);


        // Snapping to floor
        // if (!character.justJumped)
        // {
        //     Vector3 up = character.upDirection.GetNormal();
        //     // ray = Ray3(transform.GetPosition(), up * -1.f);
        //     // ray = Ray3(position, up * -1.f);
        //     float testHeight = 2.f;
        //     ray = Ray3(position - up * testHeight, up * -1.f);
        //
        //     std::unordered_set<Entity> ents_to_check = collisionSystem->QueryStaticRay(ray, character.floorSnapDistance);
        //
        //     RaycastResult closestResult;
        //     closestResult.t = FLT_MAX;
        //
        //     // We got to find the closest floor if any
        //     for (const auto& entityB : ents_to_check)
        //     {
        //         testResult.Reset();
        //         if (!coordinator.HasComponent<RigidBodyComponent>(entityB))
        //             continue;
        //
        //         const auto& rb = coordinator.GetComponent<RigidBodyComponent>(entityB);
        //
        //         // For now its only static bodies, although I may want to change this for things like moving platforms
        //         // in the future.
        //         if (!rb.isStatic)
        //             continue;
        //
        //         const auto& collB = coordinator.GetComponent<ColliderComponent>(entityB);
        //         const auto& transformB = coordinator.GetComponent<Transform3DComponent>(entityB);
        //         Transform3DComponent expandedTransformB = transformB;
        //         expandedTransformB.SetScale(transformB.GetScale() + transform.GetScale());
        //         ColliderComponent expandedCollB = collB;
        //         expandedCollB.UpdateWorldSpace(expandedTransformB);
        //
        //
        //         collisionSystem->RaycastCollider(ray, testResult, collB);
        //         // collisionSystem->RaycastCollider(ray, testResult, expandedCollB);
        //
        //         // If collision and is a floor
        //         if (testResult.hit && testResult.normal.GetDotProduct(up) > character.floorThreshold)
        //         {
        //             if (testResult.t < closestResult.t)
        //                 closestResult = testResult;
        //         }
        //     }
        //
        //
        //     // Snap to floor
        //     if (closestResult.hit)
        //     {
        //         character.isOnFloor = true;
        //
        //         // float desired = character.floorSnapDistance;
        //         // float correction = desired - closestResult.t;
        //         //
        //         // correction = std::clamp(
        //         //     correction,
        //         //     -character.floorSnapDistance,
        //         //      character.floorSnapDistance
        //         // );
        //
        //         position += up * closestResult.t;
        //         // position += up * transform.GetScale() * character.floorSnapDistance;
        //
        //     }
        //
        //     // Removing velocity into surface
        //     if (character.isOnFloor)
        //     {
        //         // Removing velocity into the ground
        //         float vDot = character.velocity.GetDotProduct(up);
        //         if (vDot < 0)
        //         {
        //             character.velocity -= up * vDot;
        //             utility::log(character.velocity.GetString());
        //         }
        //     }
        // }
        // else
        // {
        //     character.justJumped = false;
        // }

        // if (character.isOnFloor)
        // {
        //     // position += character.upDirection * character.floorSnapDistance;
        // }
        //

        // TODO: fix the slight floor sink... happens because the raycast is too small and the snap method won't work
        snapToFloor(transform.GetPosition(), position, transform.GetScale(), character, entity, deltaTime);

        if (character.isOnFloor)
        {
            // Kill downward velocity
            float vDot = character.velocity.GetDotProduct(character.upDirection);
            if (vDot < 0)
                character.velocity -= character.upDirection * vDot;
        }

        character.justJumped = false;
        // Updating final position
        transform.SetPosition(position);
    }
}

void CharacterMovementSystem::snapToFloor(const Vector3& original_pos, Vector3& position, const Vector3& coll_scale, CharacterBodyComponent& character, const Entity self, const float deltaTime)
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();
    auto collisionSystem = coordinator.GetSystem<CollisionSystem>();

    // Conditions to allow snapping
    if (character.justJumped)
    {
        // utility::log("jumped");
        return;
    }


    Vector3 up = character.upDirection.GetNormal();


    // If the slide test determined a floor collision we just need to move it up here
    // if (character.isOnFloor)
    // {
    //     // position += up * character.floorSnapDistance;
    // }
    // else
    // {
    // Only snap when falling or stationary
    if (character.velocity.GetDotProduct(up) > 0)
        return;

    // Ray origin at feet
    // Vector3 rayOrigin = position - up * character.halfHeight;
    Vector3 rayOrigin = position - up * coll_scale + up * 0.1f;
    // if (character.isOnFloor)
    // {
        // rayOrigin = position - up * coll_scale + up * 0.1f;
        // rayOrigin += up * character.floorSnapDistance;
    // }
    Ray3 ray(rayOrigin, up * -1.f);

    RaycastResult hit;
    hit.Reset();
    hit.t = character.floorSnapDistance;

    float snapLength = character.floorSnapDistance;
    float fallDistance = -character.velocity.GetDotProduct(up) * deltaTime;
    // utility::log(std::to_string(fallDistance));
    snapLength += std::max(0.0f, fallDistance);

    std::unordered_set<Entity> ents_to_check = collisionSystem->QueryStaticRay(ray, snapLength);

    // utility::log("Ents to check: " + std::to_string(ents_to_check.size()));
    for (const auto& e : ents_to_check)
    {
        if (e == self)
            continue;

        const auto& coll = coordinator.GetComponent<ColliderComponent>(e);
        const auto& tf   = coordinator.GetComponent<Transform3DComponent>(e);

        RaycastResult tmp;
        collisionSystem->RaycastCollider(ray, tmp, coll);

        if (!tmp.hit)
            continue;

        if (tmp.normal.GetDotProduct(up) < character.floorThreshold)
            continue;

        if (tmp.t < hit.t)
            hit = tmp;
    }

    if (!hit.hit)
    {
        // utility::log("Exit 1");
        return;
    }

    //SNAP
    const float desired = character.safeMargin;
    float correction = desired - hit.t;

    correction = std::clamp(
        correction,
        -character.floorSnapDistance,
         character.floorSnapDistance
    );

    position += up * correction;

    if (hit.hit)
    {
        character.isOnFloor = true;
    }
}
