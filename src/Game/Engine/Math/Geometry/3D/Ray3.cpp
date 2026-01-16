#include "Ray3.h"

// Static method
Ray3 Ray3::FromPoints(const Point3& from, const Point3& to)
{
    return Ray3(from, (to - from).GetNormal());
}


void Ray3::NormalizeDirection()
{
    direction.Normalize();
}

