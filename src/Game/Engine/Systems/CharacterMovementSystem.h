#pragma once

#include "../Core/ECS.h"
#include "../Physics/CollisionManifold.h"
#include "../Components/CharacterBodyComponent.h"
#include <vector>

// Updatees CharacterBody physics ONLY when regarding the physical world environment
// NEW: They are removed from the collision detection system to implement Godot method of move_and_slide
class CharacterMovementSystem : public System
{
public:
    void Update(float deltaTime) override;

private:
    void snapToFloor(const Vector3& original_pos, Vector3& position, const Vector3& coll_scale, CharacterBodyComponent& character, const Entity self, const float deltaTime);
private:
    static constexpr float _EPS = 0.00000001f;
};
