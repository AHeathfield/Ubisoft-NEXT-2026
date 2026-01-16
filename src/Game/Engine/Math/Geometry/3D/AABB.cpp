#include "AABB.h"
#include <cmath>

// Static method
// Given a minimum and maximum point, construct an AABB
AABB AABB::FromMinMax(const Vector3& min, const Vector3& max)
{
    return AABB((min + max) * 0.5f, (max - min)* 0.5f);
}

// Returns the minimum point of the box
Vector3 AABB::GetMin() const
{
    Vector3 p1 = position + size;
    Vector3 p2 = position - size;

    return Vector3(fminf(p1.x, p2.x), fminf(p1.y, p2.y), fminf(p1.z, p2.z));
}

// Returns the minimum point of the box
Vector3 AABB::GetMax() const
{
    Vector3 p1 = position + size;
    Vector3 p2 = position - size;

    return Vector3(fmaxf(p1.x, p2.x), fmaxf(p1.y, p2.y), fmaxf(p1.z, p2.z));
}
