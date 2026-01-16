#pragma once

#include "Point3.h"

struct Sphere
{
    Point3 position = Point3(0, 0, 0);
    float radius = 1.f;

    Sphere() = default;
    Sphere(const Point3& p, float r) :
        position(p), radius(r)
    {}
};
