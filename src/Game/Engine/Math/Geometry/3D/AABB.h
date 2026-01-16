#pragma once

#include "Point3.h"
#include "../../Vector3.h"

// Axis Aligned Bounding Box (3D Rectangle)
// Defined by center point and half extent (size)
// Ex. Half extents of (2,3,4) would be 4 units wide, 6 units tall, and 8 units deep (since we refer to the center point)
struct AABB
{
    Point3 position = Point3(0, 0, 0);
    Vector3 size = Vector3(1.f, 1.f, 1.f);

    AABB() = default;
    AABB(const Point3& position, const Vector3& halfSize) :
        position(position), size(halfSize)
    {}

    // Statics
    static AABB FromMinMax(const Vector3& min, const Vector3& max);

    Vector3 GetMin() const;
    Vector3 GetMax() const;
};
