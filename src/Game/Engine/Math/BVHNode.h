#pragma once

#include "Geometry/3D/AABB.h"
#include <array>
#include <memory>
#include <string>
#include <vector>

// I could make another BVHNode for static objects and have them be made using arrays for faster operations
// Pointers are better for many insertions and deletions.

// Another option I could do since this is an Octree implementation is have a std::unique_ptr<std::array<BVHNode, 8>> children instead
struct BVHNode
{
    AABB bounds = AABB();
    BVHNode* children = nullptr;
    // std::unique_ptr<std::array<BVHNode, 8>> children;
    std::vector<int> triangleIndices;

    ~BVHNode()
    {
        // FreeNode();
        if (children != nullptr)
        {
            delete[] children;
        }
    }

    // Recursive deletetion of node if 1 is deleted all its children must be deleted
    void FreeNode()
    {
        if (children != nullptr)
        {
            // We need to recursively (depth first) clear the data of all child nodes
            for (int i = 0; i < 8; i++)
            {
                children[i].FreeNode();
            }
            delete[] children;
            children = nullptr;
        }

        // If triangle indices are present, release the array holding them
        if (triangleIndices.size() != 0)
        {
            triangleIndices.clear();
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
