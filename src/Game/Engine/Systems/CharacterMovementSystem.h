#pragma once

#include "../Core/ECS.h"
#include "../Physics/CollisionManifold.h"
#include <vector>

// Updatees CharacterBody physics ONLY when regarding the physical world environment
// NEW: They are removed from the collision detection system to implement Godot method of move_and_slide
class CharacterMovementSystem : public System
{
public:
    void Update(float deltaTime) override;
};
