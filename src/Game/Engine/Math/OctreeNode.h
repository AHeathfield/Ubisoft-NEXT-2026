#pragma once

#include "Geometry/3D/AABB.h"
#include "../Core/ECS.h"
#include <vector>

struct OctreeNode
{
    AABB bounds = AABB();
    OctreeNode* children = nullptr;
    std::vector<Entity> entities;

    OctreeNode() : children(nullptr) {}
    ~OctreeNode()
    {
        if (children != nullptr)
        {
            delete[] children;
        }
    }

    // This is mainly for debugging purposes
    void GetAllBounds(std::vector<AABB>& allBounds)
    {
        allBounds.push_back(bounds);
        if (children != nullptr)
        {
            for (int i = 0; i < 8; i++)
            {
                children[i].GetAllBounds(allBounds);
            }
        }
    }

};
