#pragma once

#include "Vector3.h"
#include "../Graphics/Color.h"
#include <string>

// It might be nice to make the it 3 separate Vector3 a, b, and c and then have some operator overloads or something for points
struct Triangle
{
    // 3 points that define outline of triangle
    Vector3 points[3] = {
        Vector3(0, 0, 0),
        Vector3(0, 0, 0),
        Vector3(0, 0, 0)
    }; 
    Color color[3] = {
        Color(),
        Color(),
        Color()
    }; // NEW: API now provides colours for each vertex
    
    static Triangle YUpTriangle()
    {
        Triangle tri;
        tri.points[0] = Vector3(0.f, 0.f, 0.5f);     // top
        tri.points[1] = Vector3(-0.5f, 0.f, -0.5f);  // bot left
        tri.points[2] = Vector3(0.5f, 0.f, -0.5f);   // bot right
        return tri;
    }

    void SetSolidColor(const Color& solid)
    {
        color[0] = solid;
        color[1] = solid;
        color[2] = solid;
    }

    Vector3 GetCenterPoint() const
    {
        return Vector3((points[0].x + points[1].x + points[2].x) / 3, (points[0].y + points[1].y + points[2].y) / 3, (points[0].z + points[1].z + points[2].z) / 3);
    }

    void SetWindingCCW()
    {
        Vector3 edge1 = points[1] - points[0];
        Vector3 edge2 = points[2] - points[0];
        Vector3 normal = edge1.GetCrossProduct(edge2);

        // If the normal points in the negative Z direction
        if (normal.z > 0.0f)
        {
            std::swap(points[1], points[2]);
        }
    }

    std::string GetString() const
    {
        std::string info = "Point 1: " + points[0].GetString() + " || Point 2: " + points[1].GetString() + " || Point 3: " + points[2].GetString();
        return info;
    }
};
