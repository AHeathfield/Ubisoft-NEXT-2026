#pragma once

#include "Point3.h"
#include "../../Vector3.h"
#include "../../Mat3x3.h"
#include <array>

// Oriented Bounding Box
// Composed of a position, half extents, and some orientation
struct OBB
{
    Point3 position = Vector3(0, 0, 0);
    Vector3 size = Vector3(1.f, 1.f, 1.f);
    // Textbook says that no matter how we store the orientation, it will need to be converted to a matrix for rendering
    // I might change this based on how I do the rendering
    // Similar to how I would do the Transform3DComponent
    // Each row represents one of the OBB local axes (XYZ)
    // Essentially its an X-axis direction Y... Z...
    Mat3x3 orientation = Mat3x3::Identity();

    // Constructors
    OBB() = default;
    OBB(const Point3& position, const Vector3& size) :
        position(position), size(size)
    {}
    OBB(const Point3& position, const Vector3& size, const Mat3x3& orientation) :
        position(position), size(size), orientation(orientation)
    {}


    std::array<Point3, 8> GetVertices() const
    {
        std::array<Vector3, 8> v;

        Vector3 C = position; // OBB Center
        Vector3 E = size; // OBB Extents

        std::array<Vector3, 3> A = orientation.GetVector3Array(); // OBB Axis
        
        // Every vertex is the center of the plus a vector that is the extents projected onto an axis
        // No formula just figuring out logic
        v[0] = C + A[0] * E.x + A[1] * E.y + A[2] * E.z;
        v[1] = C - A[0] * E.x + A[1] * E.y + A[2] * E.z;
        v[2] = C + A[0] * E.x - A[1] * E.y + A[2] * E.z;
        v[3] = C + A[0] * E.x + A[1] * E.y - A[2] * E.z;
        v[4] = C - A[0] * E.x - A[1] * E.y - A[2] * E.z;
        v[5] = C + A[0] * E.x - A[1] * E.y - A[2] * E.z;
        v[6] = C - A[0] * E.x + A[1] * E.y - A[2] * E.z;
        v[7] = C - A[0] * E.x - A[1] * E.y + A[2] * E.z;

        return v;
    }
};
