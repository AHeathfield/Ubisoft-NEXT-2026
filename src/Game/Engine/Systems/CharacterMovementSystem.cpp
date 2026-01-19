#include "CharacterMovementSystem.h"
#include "CollisionSystem.h"
#include "../Core/GameManager.h"
#include "../Components/ColliderComponent.h"
#include "../Components/CharacterBodyComponent.h"
#include "../Components/RigidBodyComponent.h"
#include <string>
#include <unordered_map>
#include <unordered_set>


void CharacterMovementSystem::Update(float deltaTime)
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();
    auto collisionSystem = coordinator.GetSystem<CollisionSystem>();

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

        // Slides
        for (int i = 0; i < character.maxSlides; i++)
        {
            outResult.Reset();
            outResult.t = FLT_MAX;
            ray = Ray3(position, remainingMove.GetNormal());
            std::unordered_set<Entity> ents_to_check = collisionSystem->QueryStaticRay(ray, remainingMove.GetMagnitude());

            // Getting the closest raycast result
            for (const auto& entityB : ents_to_check)
            {
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

        transform.SetPosition(position);


        // Snapping to floor
        Vector3 up = character.upDirection.GetNormal();

        ray = Ray3(transform.GetPosition(), up * -1.f);

        std::unordered_set<Entity> ents_to_check = collisionSystem->QueryStaticRay(ray, character.floorSnapDistance);

        RaycastResult closestResult;
        closestResult.t = FLT_MAX;

        // We got to find the closest floor if any
        for (const auto& entityB : ents_to_check)
        {
            testResult.Reset();
            if (!coordinator.HasComponent<RigidBodyComponent>(entityB))
                continue;

            const auto& rb = coordinator.GetComponent<RigidBodyComponent>(entityB);

            // For now its only static bodies, although I may want to change this for things like moving platforms
            // in the future.
            if (!rb.isStatic)
                continue;

            const auto& collB = coordinator.GetComponent<ColliderComponent>(entityB);
            const auto& transformB = coordinator.GetComponent<Transform3DComponent>(entityB);
            Transform3DComponent expandedTransformB = transformB;
            expandedTransformB.SetScale(transformB.GetScale() + transform.GetScale());
            ColliderComponent expandedCollB = collB;
            expandedCollB.UpdateWorldSpace(expandedTransformB);


            collisionSystem->RaycastCollider(ray, testResult, collB);

            // If collision and is a floor
            if (testResult.hit && testResult.normal.GetDotProduct(up) > character.floorThreshold)
            {
                if (testResult.t < closestResult.t)
                    closestResult = testResult;
            }
        }


        // Snap to floor
        if (closestResult.hit)
        {
            // utility::log("Closest t: " + std::to_string(closestResult.t));
            character.isOnFloor = true;
            // transform.Translate((up * -1) * closestResult.t);
            transform.Translate((up) * closestResult.t);

        }

        // Removing velocity into surface
        if (character.isOnFloor)
        {
            // Removing velocity into the ground
            float vDot = character.velocity.GetDotProduct(up);
            if (vDot < 0)
            {
                character.velocity -= up * vDot;
            }
        }
    }
}
