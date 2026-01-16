#pragma once

#include "Point3.h"
#include "../../Vector3.h"

// A plane is a flat surface that extends infinitely in all directions
// Has a direction
// 3 Common ways to represent a plane
// 1. 3 Points (not on a straight line)
// 2. A normal and a point on the plane
// 3. A normal and the distance from the origin
// This implementation will use 3rd way
struct Plane
{
    // Assuming the normal given is always normalized
    // TODO: Also the TB says that this default is facing up, but this makes it seem like its facing down (will have to check this)
    Vector3 normal = Vector3(1.f, 0, 0);
    // Vector3 normal = Vector3::UP;
    float distance = 0.f;

    // Constructors
    Plane() = default;
    Plane(const Vector3& normal, float distance) :
        normal(normal), distance(distance)
    {}

    static Plane YUpPlane()
    {
        return Plane(Vector3(0, 1.f, 0), 0);
    }

    // Will return the following:
    // 0 if the point is on the plane
    // A positive number if the point is in front of the plane
    // A negative number if the point is behind the plane
    float PlaneEquation(const Point3& point) const;
};
