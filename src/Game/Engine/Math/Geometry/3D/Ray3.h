#pragma once

#include "Point3.h"
#include "../../Vector3.h"

struct RaycastResult
{
    Vector3 point = Vector3(0, 0, 0);
    Vector3 normal = Vector3::FORWARD;
    float t = -1.f;
    bool hit = false;

    void ResetRaycastResult()
    {
        t = -1;
        hit = false;
        normal = Vector3::FORWARD;
        point = Vector3();
    }
};

struct Ray3
{
    Point3 origin = Point3(0, 0, 0);
    // Assuming direction is always normalized
    Vector3 direction = Vector3(0, 0, 1.f);

    // Constructor
    Ray3() = default;
    Ray3(const Point3& origin, const Vector3& direction)
        : origin(origin), direction(direction)
    {
        NormalizeDirection();
    }

    // Statics
    static Ray3 FromPoints(const Point3& from, const Point3& to);

    void NormalizeDirection();
};
