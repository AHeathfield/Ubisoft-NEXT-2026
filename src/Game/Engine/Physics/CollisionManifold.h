#pragma once

#include "../Core/ECS.h"
#include "../Math/Vector3.h"

// Contains info about the collision
struct CollisionManifold
{
    bool isColliding = false;
    Vector3 normal = Vector3::FORWARD;
    float depth = FLT_MAX; // Amount of penetration

    std::vector<Vector3> contacts; // Contact points
 
    void Reset()
    {
        isColliding = false;
        normal = Vector3::FORWARD;
        depth = FLT_MAX;
        contacts.clear();
    }
};


