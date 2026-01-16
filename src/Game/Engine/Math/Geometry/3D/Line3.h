#pragma once

#include "Point3.h"

struct Line3
{
    Point3 start = Point3(0, 0, 0);
    Point3 end = Point3(0, 0, 0);

    Line3() = default; // Default constructor
    Line3(const Point3& start, const Point3& end)
        : start(start), end(end)
    {}

    float GetLength() const;
    float GetLengthSquared() const;
    // Common to add a GetLengthSquared as well but I don't think thats necessary
};
