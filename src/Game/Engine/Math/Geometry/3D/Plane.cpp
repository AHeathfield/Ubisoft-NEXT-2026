#include "Plane.h"

float Plane::PlaneEquation(const Point3& point) const
{
    return point.GetDotProduct(normal) - distance;
}
