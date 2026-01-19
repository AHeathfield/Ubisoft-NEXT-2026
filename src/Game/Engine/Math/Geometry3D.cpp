#include "Geometry3D.h"
#include "../Core/Utility.h"
#include "Geometry/3D/Plane.h"
#include "Geometry/3D/Point3.h"
#include "Geometry/3D/Ray3.h"
#include "Mat3x3.h"
#include "Triangle.h"
#include "Vector3.h"
#include <cmath>
#include <array>
#include <list>
#include <string>

namespace Geometry3D
{
    // ==================== Point Tests ====================
    bool IsPointInSphere(const Point3& point, const Sphere& sphere)
    {
        float magnitude_squared = (point - sphere.position).GetMagnitudeSquared();
        float radius_squared = sphere.radius * sphere.radius;

        // If squared magnitude is less than the radius squared, that means the point is inside sphere
        return magnitude_squared < radius_squared;
    }

    Point3 GetClosestPoint(const Sphere& sphere, const Point3& point)
    {
        Vector3 sphereToPoint = point - sphere.position;
        sphereToPoint.Normalize();
        sphereToPoint = sphereToPoint * sphere.radius; // This is now a point on the surface of the sphere local to the sphere
        return sphereToPoint + sphere.position; // This is point in world space
    }

    bool IsPointInAABB(const Point3& point, const AABB& aabb)
    {
        Point3 min = aabb.GetMin();
        Point3 max = aabb.GetMax();

        // Shapes don't intersect if any component of the test point is smaller than the respective component of the min point of aabb
        if (point.x<min.x || point.y<min.y ||point.z<min.z)
            return false;

        // Shapes don't intersect if any component of the test point is bigger than the respective component of the max point of aabb
        else if (point.x>max.x || point.y>max.y ||point.z>max.z)
            return false;



        return true;
    }

    // Finds closest point on the surface of the aabb given a test point
    Point3 GetClosestPoint(const AABB& aabb, const Point3& point)
    {
        Point3 result = point;
        Point3 min = aabb.GetMin();
        Point3 max = aabb.GetMax();

        // Clamp the closest point to the min point of the AABB
        if (result.x < min.x)
            result.x = min.x;
        if (result.y < min.y)
            result.y = min.y;
        if (result.z < min.z)
            result.z = min.z;

        // Clamp the closest point to the max point of the AABB
        if (result.x > max.x)
            result.x = max.x;
        if (result.y > max.y)
            result.y = max.y;
        if (result.z > max.z)
            result.z = max.z;

        return result;
    }

    bool IsPointInOBB(const Point3& point, const OBB& obb)
    {
        Vector3 dir = point - obb.position;

        // iteration 0 is X axis, 1 = Y axis, 2 = Z axis
        // This will project the point on to each of the local axes of the obb
        // It will then compare the distance of the extent of the box on that axis
        for (int i = 0; i < 3; i++)
        {
            Vector3 axis = Vector3(obb.orientation.m[i][0], obb.orientation.m[i][1], obb.orientation.m[i][2]);

            // Project the relative point on axis and record how far from origin of the box the projection is
            float distance = dir.GetDotProduct(axis);

            const std::array<float, 3> obb_size_arr = obb.size.GetArray();

            // If dist is > or < than extent of box (and negative), then its not inside box
            if (distance > obb_size_arr[i])
            {
                return false;
            }
            if (distance < -1*obb_size_arr[i])
            {
                return false;
            }
        }

        return true;
    }

    // similar to checking if its inside instead of returning false if its outside we clamp the appropriate component of point
    Point3 GetClosestPoint(const OBB& obb, const Point3& point)
    {
        Point3 result = obb.position;
        Vector3 dir = point - obb.position;

        for (int i = 0; i < 3; i++)
        {
            Vector3 axis = Vector3(obb.orientation.m[i][0], obb.orientation.m[i][1], obb.orientation.m[i][2]);

            float distance = dir.GetDotProduct(axis);

            const std::array<float, 3> obb_size_arr = obb.size.GetArray();

            if (distance > obb_size_arr[i])
            {
                distance = obb_size_arr[i];
            }
            if (distance < -1*obb_size_arr[i])
            {
                distance = -1*obb_size_arr[i];
            }

            // adjust final point by the axis and current projected distance
            result = result + (axis * distance);
        }

        return result;
    }

    // Simply compares the result of the plane equation against 0.
    bool IsPointOnPlane(const Point3& point, const Plane& plane)
    {
        float dot = point.GetDotProduct(plane.normal);

        // Macro defined in Utility.h
        return CMP_FLOAT(dot - plane.distance, 0.0f);
    }

    Point3 GetClosestPoint(const Plane& plane, const Point3& point)
    {
        float dot = point.GetDotProduct(plane.normal);
        float distance = dot - plane.distance;
        return point - plane.normal * distance;
    }

    // Uses GetClosestPoint
    bool IsPointOnLine(const Point3& point, const Line3& line)
    {
        Point3 closest = GetClosestPoint(line, point);
        float distance_squared = (closest - point).GetMagnitudeSquared();
        return CMP_FLOAT(distance_squared, 0.f);
    }

    Point3 GetClosestPoint(const Line3& line, const Point3& point)
    {
        // Projecting point onto line to find value t
        Vector3 line_vec = line.end - line.start;
        float t = (point - line.start).GetDotProduct(line_vec) / line_vec.GetDotProduct(line_vec);

        // t represents how far along line the point is
        // t is normalized, a value of less than 0 or greater than 1 falls outside the line segment.
        // That is why we clamp t into the range 0 to 1
        t = fmaxf(t, 0.f); // Clamp to 0
        t = fminf(t, 1.f); // Clamp to 1
        return line.start + line_vec * t;
    }

    bool IsPointOnRay(const Point3& point, const Ray3& ray)
    {
        // If point equals ray origin return true early
        if (point.Equals(ray.origin))
        {
            return true;
        }

        // Normal from the point we are testing to origin of ray
        Vector3 normal = point - ray.origin;
        normal.Normalize();
        // We assume the ray direction is normalized

        float diff = normal.GetDotProduct(ray.direction);

        // If BOTH vectors point in same direction,
        // their dot product (diff) should be 1
        return CMP_FLOAT(diff, 1.f);
    }

    // Finds closest point on the surface of the ray given a test point
    Point3 GetClosestPoint(const Ray3& ray, const Point3& point)
    {
        float t = (point - ray.origin).GetDotProduct(ray.direction);
        // We assume the direction of the ray is normalized
        // If for some reason the direction is not normalized
        // the below division is needed. So long as the ray
        // direction is normalized, we don't need this divide
        // t /= ray.direction.GetDotProduct(ray.direction);

        // Want to clamp t to the poistive dir
        // negative t value would put point behind the
        // origin of the array (ray extends infinitely
        // in the positive dir)
        t = fmaxf(t, 0.f);
        return Point3(ray.origin + ray.direction * t);
    }

    // Get both radiuses and then the distance between them
    // The reason its squared is because we get the distance between the squared (which is faster than getting magnitude normally)
    bool IsSphereSphereIntersection(const Sphere& s1, const Sphere& s2)
    {
        float radiiSum = s1.radius + s2.radius;
        float sqDistance = (s1.position - s2.position).GetMagnitudeSquared();
        return sqDistance < (radiiSum * radiiSum);
    }
    // Find closest point on the AABB to the center of the sphere, then we find the distance between the closest point and center of sphere, finally  we compare the radius of the sphere to the distance.
    bool IsSphereAABBIntersection(const Sphere& sphere, const AABB& aabb)
    {
        Point3 closestPoint = GetClosestPoint(aabb, sphere.position);
        float distSq = (sphere.position - closestPoint).GetMagnitudeSquared();
        float radiusSq = sphere.radius * sphere.radius;
        return distSq < radiusSq;
    }
    bool IsSphereOBBIntersection(const Sphere& sphere, const OBB& obb)
    {
        Point3 closestPoint = GetClosestPoint(obb, sphere.position);
        float distSq = (sphere.position - closestPoint).GetMagnitudeSquared();
        float radiusSq = sphere.radius * sphere.radius;
        return distSq < radiusSq;
    }
    bool IsSpherePlaneIntersection(const Sphere& sphere, const Plane& plane)
    {
        Point3 closestPoint = GetClosestPoint(plane, sphere.position);
        float distSq = (sphere.position - closestPoint).GetMagnitudeSquared();
        float radiusSq = sphere.radius * sphere.radius;
        return distSq < radiusSq;
    }


    // If all axis's overlap there is an intersection (collision)
    bool IsAABBAABBIntersection(const AABB& aabb1, const AABB& aabb2)
    {
        Point3 aMin = aabb1.GetMin();
        Point3 aMax = aabb1.GetMax();
        Point3 bMin = aabb2.GetMin();
        Point3 bMax = aabb2.GetMax();

        return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
               (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
               (aMin.z <= bMax.z && aMax.z >= bMin.z);
    }

    Interval GetInterval(const AABB& aabb, const Vector3& axis)
    {
        Vector3 i = aabb.GetMin();
        Vector3 a = aabb.GetMax();

        // 8 vertices of aabb
        Vector3 vertex[8] = {
            Vector3(i.x, a.y, a.z),
            Vector3(i.x, a.y, i.z),
            Vector3(i.x, i.y, a.z),
            Vector3(i.x, i.y, i.z),
            Vector3(a.x, a.y, a.z),
            Vector3(a.x, a.y, i.z),
            Vector3(a.x, i.y, a.z),
            Vector3(a.x, i.y, i.z)
        };

        // Projecting each vertex onto the provided axes, and storing the min and max vertices in an interval structure
        Interval result;
        result.min = result.max = axis.GetDotProduct(vertex[0]);

        for (int i = 1; i < 8; i++)
        {
            float projection = axis.GetDotProduct(vertex[i]);

            if (projection < result.min)
                result.min = projection;
            if (projection > result.max)
                result.max = projection;
        }

        return result;
    }

    Interval GetInterval(const OBB& obb, const Vector3& axis)
    {
        Vector3 vertex[8];

        // Finding center, extents, and axis of OBB
        Vector3 C = obb.position;   // OBB Center
        Vector3 E = obb.size;       // OBB Extents
        std::array<Vector3, 3> A = obb.orientation.GetVector3Array(); // OBB Axis

        // Using C,E,A to find the actual vertices
        vertex[0] = C + A[0]*E.x + A[1]*E.y + A[2]*E.z;
        vertex[1] = C - A[0]*E.x + A[1]*E.y + A[2]*E.z;
        vertex[2] = C + A[0]*E.x - A[1]*E.y + A[2]*E.z;
        vertex[3] = C + A[0]*E.x + A[1]*E.y - A[2]*E.z;
        vertex[4] = C - A[0]*E.x - A[1]*E.y - A[2]*E.z;
        vertex[5] = C + A[0]*E.x - A[1]*E.y - A[2]*E.z;
        vertex[6] = C - A[0]*E.x + A[1]*E.y - A[2]*E.z;
        vertex[7] = C - A[0]*E.x - A[1]*E.y + A[2]*E.z;

        // Projecting each vertex onto the provided axes
        Interval result;
        result.min = result.max = axis.GetDotProduct(vertex[0]);
        for (int i = 1; i < 8; i++)
        {
            float projection = axis.GetDotProduct(vertex[i]);

            if (projection < result.min)
                result.min = projection;
            if (projection > result.max)
                result.max = projection;
        }

        return result;
    }

    bool IsOverlapOnAxis(const AABB& aabb, const OBB& obb, const Vector3& axis)
    {
        Interval a = GetInterval(aabb, axis);
        Interval b = GetInterval(obb, axis);
        return ((b.min <= a.max) && (a.min <= b.max));
    }
    bool IsOverlapOnAxis(const OBB& obb1, const OBB& obb2, const Vector3& axis)
    {
        Interval a = GetInterval(obb1, axis);
        Interval b = GetInterval(obb2, axis);
        return ((b.min <= a.max) && (a.min <= b.max));
    }

    // I would see the game physics cook book on the full explanation of how this works
    // Uses SAT test on the 15 potential axes of separation (AABB first 3, OBB next 3, cross product of every rotation frame for the AABB and OBB make the final 9)
    bool IsAABBOBBIntersection(const AABB& aabb, const OBB& obb)
    {
        const Mat3x3 o = obb.orientation;
        // Creating array of the axis that each primitive is aligned to
        Vector3 test[15] = {
            Vector3(1, 0, 0),           // AABB axis 1
            Vector3(0, 1, 0),           // AABB axis 2
            Vector3(0, 0, 1),           // AABB axis 3
            Vector3(o.m[0][0], o.m[0][1], o.m[0][2]),           // OBB axis 1
            Vector3(o.m[1][0], o.m[1][1], o.m[1][2]),           // OBB axis 2
            Vector3(o.m[2][0], o.m[2][1], o.m[2][2]),           // OBB axis 3
        };  // remaining axis will be filled in later
        
        // Next nine axes are from the cross products of the rotation frames of the 2 shapes
        // Rotation frames are the first 6 elements of the test array (we know them)
        for (int i = 0; i < 3; i++)
        {
            test[6 + i * 3 + 0] = test[i].GetCrossProduct(test[0]);
            test[6 + i * 3 + 1] = test[i].GetCrossProduct(test[1]);
            test[6 + i * 3 + 2] = test[i].GetCrossProduct(test[2]);
        }

        // All 15 axes must overlap for collision/intersection to occur
        for (int i = 0; i < 15; i++)
        {
            if (IsOverlapOnAxis(aabb, obb, test[i]) == false)
                return false;   // Separating axis found
        }

        return true;    // Separating axis not found
    }

    bool IsAABBPlaneIntersection(const AABB& aabb, const Plane& plane)
    {
        // Projecting half extents of AABB onto the plane normal
        float pLen = aabb.size.x * fabsf(plane.normal.x)
                   + aabb.size.y * fabsf(plane.normal.y)
                   + aabb.size.z * fabsf(plane.normal.z);

        // Getting distance from center of AABB to the plane
        float dot = plane.normal.GetDotProduct(aabb.position);
        float dist = dot - plane.distance;

        // Intersection occurs if the distance falls within the projected side
        return fabsf(dist) <= pLen;
    }

    // Works very similar to AABB OBB collision but the first 3 axes of separation are the basis vectors of the orientation of the first OBB instead of the AABB
    bool IsOBBOBBIntersection(const OBB& obb1, const OBB& obb2)
    {
        const std::array<Vector3, 3> o1 = obb1.orientation.GetVector3Array();
        const std::array<Vector3, 3> o2 = obb2.orientation.GetVector3Array();

        Vector3 test[15] = {
            o1[0],
            o1[1],
            o1[2],
            o2[0],
            o2[1],
            o2[2]
        };

        for (int i = 0; i < 3; i++)
        {
            test[6 + i * 3 + 0] = test[i].GetCrossProduct(test[0]);
            test[6 + i * 3 + 1] = test[i].GetCrossProduct(test[1]);
            test[6 + i * 3 + 2] = test[i].GetCrossProduct(test[2]);
        }

        for (int i = 0; i < 15; i++)
        {
            if (IsOverlapOnAxis(obb1, obb2, test[i]) == false)
            {
                return false; // Separating axis found
            }
        }

        return true; // Separating axis not found
    }


    // Works similar to AABB Plane collision
    bool IsOBBPlaneIntersection(const OBB& obb, const Plane& plane)
    {
        std::array<Vector3, 3> rot = obb.orientation.GetVector3Array();
        Vector3 normal = plane.normal;

        // Projecting half extents of AABB onto the plane normal
        float pLen = obb.size.x * fabsf(normal.GetDotProduct(rot[0]))
                   + obb.size.y * fabsf(normal.GetDotProduct(rot[1]))
                   + obb.size.z * fabsf(normal.GetDotProduct(rot[2]));

        // Getting distance from center of OBB to the plane
        float dot = plane.normal.GetDotProduct(obb.position);
        float dist = dot - plane.distance;

        // Intersection occurs if the distance falls within the projected side
        return fabsf(dist) <= pLen;
    }


    // Super simple test, if normals are not pointing in same direction then the plane intersects
    bool IsPlanePlaneIntersection(const Plane& plane1, const Plane& plane2)
    {
        // Gets direction of intersection line
        // Cross product returns 0 when used on parrallel lines
        Vector3 d = plane1.normal.GetCrossProduct(plane2.normal);

        // Check length of new vector, 0 means planes are parallel
        return CMP_FLOAT(d.GetDotProduct(d), 0.f) == false;
    }


    // Negative values mean no collision
    bool Raycast(const Sphere& sphere, const Ray3& ray, RaycastResult& outResult)
    {
        outResult.Reset();

        // Vector from origin of ray to center of sphere
        Vector3 e = sphere.position - ray.origin;

        float rSq = sphere.radius * sphere.radius;
        float eSq = e.GetMagnitudeSquared();

        // Project vector pointing from the origin of the ray to the sphere onto direction of the ray
        // ray.direction is assumed to be normalized
        float a = e.GetDotProduct(ray.direction);

        // Construct sides of a triangle using radius of circle at the projected point (above). Sides are radius b and f and we are working with squared units
        // I would see the Cook book TB for pictures on how this works
        float bSq = eSq - (a*a);
        float f = std::sqrt(rSq - bSq);

        // Store intersection time as t
        float t = a - f; // Assume normal intersection

        // Compare the length of the squared radius against the hypotenuse of the triangle.
        // No collision has happened
        if (rSq - (eSq - (a*a)) < 0.f)
            return false;      // -1 is invalid
        else if (eSq < rSq)
            t = a + f;   // Just reverse direction
        
        // Filling in the raycast values
        outResult.t = t;
        outResult.hit = true;
        outResult.point = ray.origin + ray.direction * t;
        outResult.normal = (outResult.point - sphere.position).GetNormal();

        return true;
    }

    // This uses a clipping algorithm called Cyrus-Beck Clipping.
    // Clip the ray against each of the 6 planes that make up the AABB
    // The Cookbook TB explains this well.
    bool Raycast(const AABB& aabb, const Ray3& ray, RaycastResult& outResult)
    {
        Vector3 min = aabb.GetMin();
        Vector3 max = aabb.GetMax();

        // Finding both intersections of the ray against each of the 3 slabs which make up a bounding box
        float t[6] = {0, 0, 0, 0, 0, 0};

        // Safety checks
        if (CMP_FLOAT(ray.direction.x, 0.f))
        {
            // Ray is parallel and outside the box therefore will never intersect the box
            if (ray.origin.x < min.x || ray.origin.x > max.x)
                return false;

            // Remember it doesn't really matter what these values are, we just want to know if 
            // its parallel is it atleast going into the box?
            t[0] = -FLT_MAX;
            t[1] = FLT_MAX;
        }
        else
        {
            t[0] = (min.x - ray.origin.x) / ray.direction.x;
            t[1] = (max.x - ray.origin.x) / ray.direction.x;
        }
        if (CMP_FLOAT(ray.direction.y, 0.f))
        {
            if (ray.origin.y < min.y || ray.origin.y > max.y)
                return false;

            t[2] = -FLT_MAX;
            t[3] = FLT_MAX;

        }
        else
        {
            t[2] = (min.y - ray.origin.y) / ray.direction.y;
            t[3] = (max.y - ray.origin.y) / ray.direction.y;
        }
        if (CMP_FLOAT(ray.direction.z, 0.f))
        {
            if (ray.origin.z < min.z || ray.origin.z > max.z)
                return false;

            t[4] = -FLT_MAX;
            t[5] = FLT_MAX;

        }
        else
        {
            t[4] = (min.z - ray.origin.z) / ray.direction.z;
            t[5] = (max.z - ray.origin.z) / ray.direction.z;
        }


        // Finding largest min value
        float tmin = fmaxf(
                fmaxf(
                    fminf(t[0], t[1]),
                    fminf(t[2], t[3])
                ),
                fminf(t[4], t[5])
        );

        // Finding smallest max value
        float tmax = fminf(
                fminf(
                    fmaxf(t[0], t[1]),
                    fmaxf(t[2], t[3])
                ),
                fmaxf(t[4], t[5])
        );

        // If tmax is < 0, the ray is intersecting AABB in negative direction.
        // Means the entire AABB is behind the origin of the ray and should not be treated as a intersection
        if (tmax < 0)
            return false;

        // Ray does not intersect
        if (tmin > tmax)
            return false;

        // If tmin is < 0, that means the ray intersects the AABB but its origin is inside the AABB.
        // This means tmax is the valid collision point
        float t_result = tmin;
        if (tmin < 0.f)
            t_result = tmax;

        // For Collision testing with rays and character bodies, using tmin just gives better results
        // Especially with Godots slide
        // outResult.t = t_result;
        outResult.t = tmin;
        outResult.hit = true;
        outResult.point = ray.origin + ray.direction * tmin; //t_result;
        Vector3 normals[6] = {
            Vector3(-1.f, 0, 0),
            Vector3(1.f, 0, 0),
            Vector3(0, -1.f, 0),
            Vector3(0, 1.f, 0),
            Vector3(0, 0, -1.f),
            Vector3(0, 0, 1.f)
        };

        for (int i = 0; i < 6; i++)
        {
            if (CMP_FLOAT(t_result, t[i]))
            {
                outResult.normal = normals[i];
            }
        }

        return true;
    }


    // Similar to AABB raycast
    bool Raycast(const OBB& obb, const Ray3& ray, RaycastResult& outResult)
    {
        outResult.Reset();

        const std::array<float, 3> size = obb.size.GetArray();

        // Want to store the half extents of the box as a linear array, and each axis of the OBBs rotation frame as a vector
        Vector3 X = Vector3(obb.orientation.m[0][0], obb.orientation.m[0][1], obb.orientation.m[0][2]);
        Vector3 Y = Vector3(obb.orientation.m[1][0], obb.orientation.m[1][1], obb.orientation.m[1][2]);
        Vector3 Z = Vector3(obb.orientation.m[2][0], obb.orientation.m[2][1], obb.orientation.m[2][2]);

        // To test slabs we need vector pointing from the origin of the ray to the OBB
        Vector3 p = obb.position - ray.origin;

        // Project the direction of the ray onto each of the axis of the OBB
        std::array<float, 3> f = {
                X.GetDotProduct(ray.direction),
                Y.GetDotProduct(ray.direction),
                Z.GetDotProduct(ray.direction)
        };

        // Project p into every axis of the OBBs rotation frame.
        std::array<float, 3> e = {
                X.GetDotProduct(p),
                Y.GetDotProduct(p),
                Z.GetDotProduct(p)
        };

        // Calculating tminx, tmaxx, tminy, tmaxy, tminz, tmaxz which are in respect to t[0], t[1], t[2], t[3], t[4], t[5]
        float t[6] = {0, 0, 0, 0, 0, 0};
        for (int i = 0; i < 3; i++)
        {
            if (CMP_FLOAT(f[i], 0.f))
            {
                // If the ray is parallel to the slab being tested and the origin of the ray is not inside the slab we have no hit
                if (-e[i] - size[i] > 0 || -e[i] + size[i] < 0)
                {
                    return false;
                }
                f[i] = 0.00001f; // to avoid div by 0
            }
            t[i * 2 + 0] = (e[i] + size[i]) / f[i]; // min
            t[i * 2 + 1] = (e[i] - size[i]) / f[i]; // max
        }

        // If above loop finished exe, the ray hit all 3 slabs, to finish raycast we find the largest min tmin and the smallest maximum tmax.
        float tmin = fmaxf(
                fmaxf(
                    fminf(t[0], t[1]),
                    fminf(t[2], t[3])
                ),
                fminf(t[4], t[5])
        );

        float tmax = fminf(
                fminf(
                    fmaxf(t[0], t[1]),
                    fmaxf(t[2], t[3])
                ),
                fmaxf(t[4], t[5])
        );

        // If tmax < 0, ray is intersecting the OBB in negative dir meaning the OBB is behind origin of ray
        if (tmax < 0)
            return false;

        if (tmin > tmax)
            return false;

        // tmin < 0 ray started inside OBB and tmax is valid intersection
        float t_result = tmin;
        if (tmin < 0.0f)
            t_result = tmax;

        outResult.hit = true;
        outResult.t = t_result;
        outResult.point = ray.origin + ray.direction * t_result;
        Vector3 normals[6] = {
            X,
            X * -1.f,
            Y,
            Y * -1.f,
            Z,
            Z * -1.f
        };

        for (int i = 0; i < 6; i++)
        {
            if (CMP_FLOAT(t_result, t[i]))
            {
                outResult.normal = normals[i].GetNormal();
            }
        }

        return true;
    }


    // any point on a ray at a time: point(t) = ray.origin + ray.direction * t
    // This utilizes the plane equation: Dot(point(t), plane.normal) - plane.distance = 0
    // The idea is to solve for the unknown t which satisfies the plane equation
    bool Raycast(const Plane& plane, const Ray3& ray, RaycastResult& outResult)
    {
        outResult.Reset();

        float nd = ray.direction.GetDotProduct(plane.normal);
        float pn = ray.origin.GetDotProduct(plane.normal);

        // The nd variable must be less < 0 for the ray to intersect the plane, if not the ray and plane normals point in the same direction and there is no intersection
        if (nd >= 0.f)
            return false;

        float t = (plane.distance - pn) / nd;

        // if t < 0, the ray hits the plane behind its origin (not technically a hit)
        if (t >= 0.f)
        {
            outResult.t = t;
            outResult.hit = true;
            outResult.point = ray.origin + ray.direction * t;
            outResult.normal = plane.normal.GetNormal();

            return true;
        }

        return false;
    }


    bool IsLineIntersection(const Sphere& sphere, const Line3& line)
    {
        // Find closest point on line to the sphere, then we see if the distance from the sphere center to the closest point is <= the radius, if so that point is colliding
        Point3 closest = GetClosestPoint(line, sphere.position);
        float distSq = (sphere.position - closest).GetMagnitudeSquared();
        return distSq <= (sphere.radius * sphere.radius);
    }

    bool IsLineIntersection(const AABB& aabb, const Line3& line)
    {
        // Going to do a raycast and then see if it collides if so we just need to check if t is less than or = length of line
        Ray3 ray;
        ray.origin = line.start;
        ray.direction = (line.end - line.start).GetNormal();
        RaycastResult raycast;
        if (Raycast(aabb, ray, raycast) == false)
            return false;

        float t = raycast.t;

        return (t >= 0) && (t*t <= line.GetLengthSquared());
    }


    bool IsLineIntersection(const OBB& obb, const Line3& line)
    {
        // Same method as AABB version
        Ray3 ray;
        ray.origin = line.start;
        ray.direction = (line.end - line.start).GetNormal();
        RaycastResult raycast;
        if (Raycast(obb, ray, raycast) == false)
            return false;

        float t = raycast.t;

        return (t >= 0) && (t*t <= line.GetLengthSquared());
    }


    // A line segment represented by end points A and B can be expressed as: S(t) = A + t(B-A) where 0 <= t <= 1
    // Idea is to substitute the equation above into the Plane equation so that if any point along the line at time t exists that satisfies the plane equation there is a collision
    bool IsLineIntersection(const Plane& plane, const Line3& line)
    {
        Vector3 ab = line.end - line.start;

        float nA = plane.normal.GetDotProduct(line.start);
        float nAB = plane.normal.GetDotProduct(ab);

        // If the line and plane are parrallel, nAB will be 0
        // This will cause a divide by 0 exception
        // If the positions are exactly equal it technically is colliding everywhere but that is extrememly rare especially with floating point numbers
        // So if they are parallel and positions are same we will still say that is not a collision
        if (CMP_FLOAT(nAB, 0.f))
            return false;

        float t = (plane.distance - nA) / nAB;
        return (t >= 0.f) && (t <= 1.f);
    }


    // See TB for explanation on how it works since its hard to visualize with just words
    bool IsPointInTriangle(const Point3& p, const Triangle& t)
    {
        // Create a temp triangle with the size of the original triangle in the local coordinate system of the point
        // This is so the point will be at the origin of the temporary triangle
        Vector3 a = t.points[0] - p;
        Vector3 b = t.points[1] - p;
        Vector3 c = t.points[2] - p;

        // The point should be moved too, so they are both
        // relative, but because we don't use p in the
        // equation anymore, we don't need it
        // p -= p; would just equal the 0 vector

        // Given point p and triangle ABC, create the sides of a pyramid.
        // The sides of the pyramid will be triangles created from the points: PBC, PCA, PAB.
        // Then find and store the normal of each side of this pyramid
        Vector3 normPBC = b.GetCrossProduct(c); // Normal of PBC (u)
        Vector3 normPCA = c.GetCrossProduct(a); // Normal of PCA (v)
        Vector3 normPAB = a.GetCrossProduct(b); // Normal of PAB (w)

        // If faces of the pyramid do not have the same normal, the point is not contained within the triangle
        if (normPBC.GetDotProduct(normPCA) < 0.f)
            return false;
        else if (normPBC.GetDotProduct(normPAB) < 0.f)
            return false;

        // If all faces of the pyramid have the same normal, the pyramid is flat
        // Which means the point is in the triangle and there is an intersection
        return true;
    }


    // Helper method to get the plane of a triangle
    Plane GetTrianglePlane(const Triangle& t)
    {
        Plane result;

        // Normal of the triangle will be the normal of the plane
        result.normal = (t.points[1] - t.points[0]).GetCrossProduct(t.points[2] - t.points[0]);
        result.normal.Normalize();

        // Project any point onto the normal of the plane to get the distance of the plane from the origin
        result.distance = result.normal.GetDotProduct(t.points[0]);
        return result;
    }


    // Idea is we construct a plane from the triangle then find the closest point to the test point on the plane.
    // If closest is within the triangle thats the closest point.
    // If its outsde, we construct 3 lines from the edges of the triangle and then we find the closest point to the test point along the 3 lines
    Point3 GetClosestPoint(const Triangle& triangle, const Point3& point)
    {
        Plane plane = GetTrianglePlane(triangle);
        Point3 closest = GetClosestPoint(plane, point);

        // if the point is inside the triangle, return it as the closest point
        if (IsPointInTriangle(closest, triangle))
            return closest;

        // Constructing 1 line for each side of triangle and then finding the closest point on the side of the triangle to the test point
        Point3 c1 = GetClosestPoint(Line3(triangle.points[0], triangle.points[1]), point); // Line AB
        Point3 c2 = GetClosestPoint(Line3(triangle.points[1], triangle.points[2]), point); // Line BC
        Point3 c3 = GetClosestPoint(Line3(triangle.points[2], triangle.points[0]), point); // Line CA

        // Measure how far each of the closest points from the previous step are from the test point
        float magSq1 = (point - c1).GetMagnitudeSquared();
        float magSq2 = (point - c2).GetMagnitudeSquared();
        float magSq3 = (point - c3).GetMagnitudeSquared();

        if (magSq1 < magSq2 && magSq1 < magSq3)
            return c1;
        else if (magSq2 < magSq1 && magSq2 < magSq3)
            return c2;

        return c3;
    }


    bool IsTriangleSphereIntersection(const Triangle& t, const Sphere& s)
    {
        // Checks if closest point on triangle is in sphere
        Point3 closest = GetClosestPoint(t, s.position);
        float magSq = (closest - s.position).GetMagnitudeSquared();
        return magSq <= s.radius * s.radius;
    }


    Interval GetInterval(const Triangle& triangle, const Vector3& axis)
    {
        Interval result;

        // Project the first point of the triangle onto the axis and store it as both the min and max of the interval
        result.min = axis.GetDotProduct(triangle.points[0]);
        result.max = result.min;

        // Project the remaining 2 points of the triangle onto the axis
        // If the projected point is less than min or greater to max store it accordingly:
        for (int i = 1; i < 3; i++)
        {
            float value = axis.GetDotProduct(triangle.points[i]);
            result.min = fminf(result.min, value);
            result.max = fmaxf(result.max, value);
        }

        return result;
    }


    bool IsOverlapOnAxis(const AABB& aabb, const Triangle& triangle, const Vector3& axis)
    {
        Interval a = GetInterval(aabb, axis);
        Interval b = GetInterval(triangle, axis);
        return ((b.min <= a.max) && (a.min <= b.max));
    }


    // Implemented using SAT test
    bool IsTriangleAABBIntersection(const Triangle& t, const AABB& aabb)
    {
        // Finding edge vectors of triangle
        Vector3 f0 = t.points[1] - t.points[0];
        Vector3 f1 = t.points[2] - t.points[1];
        Vector3 f2 = t.points[0] - t.points[2];

        // Finding the face normals of the AABB
        Vector3 u0 = Vector3(1.f, 0.f, 0.f);
        Vector3 u1 = Vector3(0.f, 1.f, 0.f);
        Vector3 u2 = Vector3(0.f, 0.f, 1.f);

        Vector3 test[13] = {
            u0, // AABB Axis 1
            u1, // AABB Axis 2
            u2, // AABB Axis 3
            f0.GetCrossProduct(f1), // Normal of Triangle Axis 4
            // Next 9 axis are the cross products of every normal of the ABB with every edge of the triangle
            u0.GetCrossProduct(f0), u0.GetCrossProduct(f1), u0.GetCrossProduct(f2),
            u1.GetCrossProduct(f0), u1.GetCrossProduct(f1), u1.GetCrossProduct(f2),
            u2.GetCrossProduct(f0), u2.GetCrossProduct(f1), u2.GetCrossProduct(f2),
        };

        // Testing every axis to check if there is an overlap or not.
        // If an axis of separation is found we return false
        for (int i = 0; i < 13; i++)
        {
            if (!IsOverlapOnAxis(aabb, t, test[i]))
                return false; // Separating axis found
        }

        return true; // Separating axis not found
    }


    bool IsOverlapOnAxis(const OBB& obb, const Triangle& triangle, const Vector3& axis)
    {
        Interval a = GetInterval(obb, axis);
        Interval b = GetInterval(triangle, axis);
        return ((b.min <= a.max) && (a.min <= b.max));
    }


    bool IsTriangleOBBIntersection(const Triangle& t, const OBB& obb)
    {
        // Compute the edge vectors of the triangle (ABC)
        Vector3 f0 = t.points[1] - t.points[0];
        Vector3 f1 = t.points[2] - t.points[1];
        Vector3 f2 = t.points[0] - t.points[2];

        // Store face normals of the OBB as vectors
        std::array<Vector3, 3> u = obb.orientation.GetVector3Array();

        Vector3 test[13] = {
            u[0],   // OBB Axis 1
            u[1],   // OBB Axis 2
            u[2],   // OBB Axis 3
            f0.GetCrossProduct(f1), // Normal of triangle
            // Last 9 axes are the cross product of the the normals of the OBB with edges of triangle
            u[0].GetCrossProduct(f0), u[0].GetCrossProduct(f1), u[0].GetCrossProduct(f2),
            u[1].GetCrossProduct(f0), u[1].GetCrossProduct(f1), u[1].GetCrossProduct(f2),
            u[2].GetCrossProduct(f0), u[2].GetCrossProduct(f1), u[2].GetCrossProduct(f2)
        };

        // Testing every axis to check if there is an overlap or not.
        // If an axis of separation is found we return false
        for (int i = 0; i < 13; i++)
        {
            if (!IsOverlapOnAxis(obb, t, test[i]))
                return false; // Separating axis found
        }

        return true; // Separating axis not found
    }


    // If all 3 points are on the plane, the triangle and plane are coplanar and colliding
    // If all 3 points of the triangle are in front of the plane, the triangle does not cross the plane. This case there is no collision
    // If all 3 points of the triangle are behind the plane, the triangle does not cross the plane. This case there is no collision
    bool IsTrianglePlaneIntersection(const Triangle& t, const Plane& p)
    {
        // Checking which side of the plane every point of triangle is on
        float side1 = p.PlaneEquation(t.points[0]);
        float side2 = p.PlaneEquation(t.points[1]);
        float side3 = p.PlaneEquation(t.points[2]);

        // if all points are on the plane, that is if the triangle and plane are coplanar, they intersect
        if (CMP_FLOAT(side1, 0.f) && CMP_FLOAT(side2, 0.f) && CMP_FLOAT(side3, 0.f))
            return true;

        // If all 3 points of the triangle are in front of the plane, the triangle and plane don't intersect
        if (side1 < 0.f && side2 < 0.f && side3 < 0.f)
            return false;

        return true; // Means 1 vertex is on the opposite side of the triangle as the other 2
    }


    bool IsOverlapOnAxis(const Triangle& t1, const Triangle& t2, const Vector3& axis)
    {
        Interval a = GetInterval(t1, axis);
        Interval b = GetInterval(t2, axis);
        return ((b.min <= a.max) && (a.min <= b.max));
    }


    bool IsTriangleTriangleIntersection(const Triangle& t1, const Triangle& t2)
    {
        Vector3 t1_f0 = t1.points[1] - t1.points[0]; // Triangle 1, Edge 0
        Vector3 t1_f1 = t1.points[2] - t1.points[1]; // Triangle 1, Edge 1
        Vector3 t1_f2 = t1.points[0] - t1.points[2]; // Triangle 1, Edge 2

        Vector3 t2_f0 = t2.points[1] - t2.points[0]; // Triangle 1, Edge 0
        Vector3 t2_f1 = t2.points[2] - t2.points[1]; // Triangle 1, Edge 1
        Vector3 t2_f2 = t2.points[0] - t2.points[2]; // Triangle 1, Edge 2

        // Array of potentially separating axes
        Vector3 axisToTest[] = {
            t1_f0.GetCrossProduct(t1_f1), // Normal of triangle 1
            t2_f0.GetCrossProduct(t2_f1), // Normal of triangle 2
            // Next 9 axes are the cross products of every edge of triangle 1 with every edge of triangle 2
            t2_f0.GetCrossProduct(t1_f0), t2_f0.GetCrossProduct(t1_f1),
            t2_f0.GetCrossProduct(t1_f2), t2_f1.GetCrossProduct(t1_f0),
            t2_f1.GetCrossProduct(t1_f1), t2_f1.GetCrossProduct(t1_f2),
            t2_f2.GetCrossProduct(t1_f0), t2_f2.GetCrossProduct(t1_f1),
            t2_f2.GetCrossProduct(t1_f2)
        };

        // Checking all axis to test to see if any of them are separate (no overlap)
        for (int i = 0; i < 11; i++)
        {
            if (!IsOverlapOnAxis(t1, t2, axisToTest[i]))
                return false; // Separating axis found
        }

        return true; // No separating axis found
    }


    Vector3 GetSATCrossEdge(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d)
    {
        // Create the default sides and take their cross product
        Vector3 ab = a - b;
        Vector3 cd = c - d;
        Vector3 result = ab.GetCrossProduct(cd);

        // If magnitude of the cross product is not 0, sides are not parallel, so we can just return the result
        if (!CMP_FLOAT(result.GetMagnitudeSquared(), 0.f))
        {
            return result; // Not parallel!
        }
        else // ab and cd are parallel
        {
            // Construct a temporary axis perpendicular to AB and try taking the cross product again
            Vector3 axis = ab.GetCrossProduct(c - a);
            result = ab.GetCrossProduct(axis);

            // if magnitude is not 0, the perpendicular axis produced valid results
            if (!CMP_FLOAT(result.GetMagnitudeSquared(), 0.f))
                return result; // Not parallel
        }

        // If magnitude of new cross product is 0, both triangles are coplanar and there is no way to get a proper cross product out of them
        return Vector3();
    }


    // This is a more robust/accurate version of the other IsTriangleTriangleIntersection method
    // TODO: Check the speed difference
    bool IsTriangleTriangleIntersectionRobust(const Triangle& t1, const Triangle& t2)
    {
        Vector3 axisToTest[] = {
            // Triangle 1, Normal
            GetSATCrossEdge(t1.points[0], t1.points[1], t1.points[1], t1.points[2]),
            // Triangle 2, Normal
            GetSATCrossEdge(t2.points[0], t2.points[1], t2.points[1], t2.points[2]),

            // Instead of computing cross for every edge pair we use the GetSAT helper function (since it also handles edge cases of triangle sides being parallel and producing a cross prod w 0 length)

            GetSATCrossEdge(t2.points[0], t2.points[1], t1.points[0], t1.points[1]),
            GetSATCrossEdge(t2.points[0], t2.points[1], t1.points[1], t1.points[2]),
            GetSATCrossEdge(t2.points[0], t2.points[1], t1.points[2], t1.points[0]),


            GetSATCrossEdge(t2.points[1], t2.points[2], t1.points[0], t1.points[1]),
            GetSATCrossEdge(t2.points[1], t2.points[2], t1.points[1], t1.points[2]),
            GetSATCrossEdge(t2.points[1], t2.points[2], t1.points[2], t1.points[0]),

            GetSATCrossEdge(t2.points[2], t2.points[0], t1.points[0], t1.points[1]),
            GetSATCrossEdge(t2.points[2], t2.points[0], t1.points[1], t1.points[2]),
            GetSATCrossEdge(t2.points[2], t2.points[0], t1.points[2], t1.points[0])
        };

        // Just like any other SAT test, we see if there is a separating axis (if so no collision)
        for (int i = 0; i < 11; i++)
        {
            if (!IsOverlapOnAxis(t1, t2, axisToTest[i]))
            {
                if (!CMP_FLOAT(axisToTest->GetMagnitudeSquared(), 0.f))
                {
                    return false; // Separating axis found
                }
            }
        }

        return true; // Separating axis not found
    }


    // Good video explaining Barycentric coords https://www.youtube.com/watch?v=HYAgJN3x4GA
    Vector3 GetBarycentric(const Point3& p, const Triangle& t)
    {
        // Find vectors from the test point to each point of the triangle
        Vector3 ap = p - t.points[0];
        Vector3 bp = p - t.points[1];
        Vector3 cp = p - t.points[2];

        // Find and store the edges of the triangle
        Vector3 ab = t.points[1] - t.points[0];
        Vector3 ac = t.points[2] - t.points[0];
        Vector3 bc = t.points[2] - t.points[1];
        Vector3 cb = t.points[1] - t.points[2];
        Vector3 ca = t.points[0] - t.points[2];

        // vector v will be perpendicular to edge AB. The test point is projected onto v.
        // The value of a = 0 if the projected point is on line AB.
        // Value of a = 1 if the projected point ias at point C of the triangle
        Vector3 v = ab - ab.GetProjection(cb);
        float a = 1.f - (v.GetDotProduct(ap) / v.GetDotProduct(ab));

        // v will now be perpendicular to edge BC
        v = bc - bc.GetProjection(ac);
        float b = 1.f - (v.GetDotProduct(bp) / v.GetDotProduct(bc));

        // v will now be perpendicular to edge CA
        v = ca - ca.GetProjection(ab);
        float c = 1.f - (v.GetDotProduct(cp) / v.GetDotProduct(ca));

        return Vector3(a, b, c);
    }


    // Read the cookbook TB for a better explanation of how this works
    // Also we dont need to check if point is in triangle since barycentric coordinates will tell us if it does
    bool Raycast(const Triangle& triangle, const Ray3& ray, RaycastResult& outResult)
    {
        outResult.Reset();

        // Create a plane a cast the ray against the plane
        // If ray does not hit the plane the ray will not hit the triangle
        Plane plane = GetTrianglePlane(triangle);
        RaycastResult raycast_plane;
        if (Raycast(plane, ray, raycast_plane) == false)
            return false;

        // Find the point on the plane where the ray hit
        float t = raycast_plane.t;
        Point3 result = ray.origin + ray.direction * t;

        // Find the barcentric coords of the Raycast on the plane
        // If this point is within the triangle, the ray hit the triangle
        Vector3 barycentric = GetBarycentric(result, triangle);
        if (barycentric.x >= 0.f && barycentric.x <= 1.f &&
            barycentric.y >= 0.f && barycentric.y <= 1.f &&
            barycentric.z >= 0.f && barycentric.z <= 1.f)
        {
            outResult.t = t;
            outResult.hit = true;
            outResult.point = ray.origin + ray.direction * t;
            outResult.normal = plane.normal;

            return true;
        }

        return false;
    }


    bool IsLineIntersection(const Triangle& triangle, const Line3& line)
    {
        // Constructing ray out of line being tested
        Ray3 ray;
        ray.origin = line.start;
        ray.direction = (line.end - line.start).GetNormal();

        // Perform a raycast
        RaycastResult raycast;
        if (Raycast(triangle, ray, raycast))
            return false;

        float t = raycast.t;

        // Check that result of the Raycast is within the size of the line
        return (t >= 0) && (t*t <= line.GetLengthSquared());
    }

    // Loops through every vertex of the mesh being accelerated.
    // It finds the min and max points of the mesh and creates a BVHNode using the min and max points as the bounding volume.
    // Every triangle of the mesh is put into this root node
    // Node is split 3 times using the SplitBVHNode helper method
    void AccelerateMesh(Mesh& mesh)
    {
        if (mesh.accelerator != nullptr)
            return;

        // Finding min and max points of the mesh, can be later used to construct AABB
        Vector3 min = mesh.triangles[0].points[0];
        Vector3 max = mesh.triangles[0].points[0];
        for (int i = 1; i < mesh.triangles.size(); i++)
        {
            min.x = fminf(mesh.triangles[i].points[0].x, min.x);
            min.x = fminf(mesh.triangles[i].points[1].x, min.x);
            min.x = fminf(mesh.triangles[i].points[2].x, min.x);
            min.y = fminf(mesh.triangles[i].points[0].y, min.y);
            min.y = fminf(mesh.triangles[i].points[1].y, min.y);
            min.y = fminf(mesh.triangles[i].points[2].y, min.y);
            min.z = fminf(mesh.triangles[i].points[0].z, min.z);
            min.z = fminf(mesh.triangles[i].points[1].z, min.z);
            min.z = fminf(mesh.triangles[i].points[2].z, min.z);
            max.x = fmaxf(mesh.triangles[i].points[0].x, max.x);
            max.x = fmaxf(mesh.triangles[i].points[1].x, max.x);
            max.x = fmaxf(mesh.triangles[i].points[2].x, max.x);
            max.y = fmaxf(mesh.triangles[i].points[0].y, max.y);
            max.y = fmaxf(mesh.triangles[i].points[1].y, max.y);
            max.y = fmaxf(mesh.triangles[i].points[2].y, max.y);
            max.z = fmaxf(mesh.triangles[i].points[0].z, max.z);
            max.z = fmaxf(mesh.triangles[i].points[1].z, max.z);
            max.z = fmaxf(mesh.triangles[i].points[2].z, max.z);
        }


        // Create new mesh.accelerator structure within the mesh, set the AABB bounds to the min and max points of the mesh
        mesh.accelerator = new BVHNode();
        // min.y = min.y * -1.f;
        // max.y = max.y * -1.f;
        mesh.accelerator->bounds = AABB::FromMinMax(min, max);

        // Allocates memory for the triangle indices instead of duplicating the mesh mesh.triangles we just store indices to them
        mesh.accelerator->triangleIndices.resize(mesh.triangles.size());
        for (int i = 0; i < mesh.triangles.size(); i++)
        {
            mesh.accelerator->triangleIndices[i] = i;
        }

        SplitBVHNode(mesh.accelerator, mesh, 3);
    }


    void SplitBVHNode(BVHNode* node, const Mesh& mesh, int depth)
    {
        if (depth == 0)
            return;

        depth--;

        if (node->children == nullptr)
        {
            // If node is a leaf (no children) split it into 8 childs nodes
            if (node->triangleIndices.size() > 0)
            {
                // Allocating memory for the children of this node
                node->children = new BVHNode[8];

                // Set the extents of each child node. All children share the center point of the current node
                Vector3 c = node->bounds.position;
                Vector3 e = node->bounds.size * 0.5f;

                node->children[0].bounds = AABB(c + Vector3(-e.x, +e.y, -e.z), e);
                node->children[1].bounds = AABB(c + Vector3(+e.x, +e.y, -e.z), e);
                node->children[2].bounds = AABB(c + Vector3(-e.x, +e.y, +e.z), e);
                node->children[3].bounds = AABB(c + Vector3(+e.x, +e.y, +e.z), e);
                node->children[4].bounds = AABB(c + Vector3(-e.x, -e.y, -e.z), e);
                node->children[5].bounds = AABB(c + Vector3(+e.x, -e.y, -e.z), e);
                node->children[6].bounds = AABB(c + Vector3(-e.x, -e.y, +e.z), e);
                node->children[7].bounds = AABB(c + Vector3(+e.x, -e.y, +e.z), e);
            }
        }

        // If node was just split (if it has children), assign each child node its own list of triangles that the child node intersects
        if (node->children != nullptr && node->triangleIndices.size() > 0)
        {
            // Distribute triangles to children
            for (int i = 0; i < 8; i++)
            {
                // Fore each triangle in parent, check if it intersects child
                for (int j = 0; j < node->triangleIndices.size(); j++)
                {
                    Triangle t = mesh.triangles[node->triangleIndices[j]];
                    if (Geometry3D::IsTriangleAABBIntersection(t, node->children[i].bounds))
                    {
                        node->children[i].triangleIndices.push_back(node->triangleIndices[j]);
                    }
                }
                // Figuring out how many triangles each child will contain
                // int numOfTriangles = 0;
                // for (int j = 0; j < node->triangleIndices.size(); j++)
                // {
                //     Triangle t = mesh.triangles[node->triangleIndices[j]];
                //     if (Geometry3D::IsTriangleAABBIntersection(t, node->children[i].bounds))
                //     {
                //         numOfTriangles++;
                //     }
                // }
                //
                // // If no triangles being processed do nothing
                // if (numOfTriangles == 0)
                //     continue;
                //
                // // Allocate new memory for the indices of the child node
                // node->children[i].triangleIndices.resize(numOfTriangles);
                //
                // // Any triangle which intersects the child node being created, add its index to the list of triangle indices
                // int index = 0;
                // for (int j = 0; j < numOfTriangles; j++)
                // {
                //     Triangle t = mesh.triangles[node->triangleIndices[j]];
                //     if (Geometry3D::IsTriangleAABBIntersection(t, node->children[i].bounds))
                //     {
                //         node->children[i].triangleIndices[index] = node->triangleIndices[j];
                //         index++;
                //     }
                // }

            }

            // Clear parents triangle indices after distributing to children
            node->triangleIndices.clear();

            // NOTE: I changed triangles to triangleIndices and used a std::vector so we don't need to memory manage and we don't need to store the numTriangles
            // Cleanup by removing any triangles that this node might have been holding onto
            // node->numTriangles = 0;
            // delete[] node->triangles;
            // node->triangles = 0;

            // Recursive calls
            for (int i = 0; i < 8; i++)
            {
                // Only recurse if child has triangles
                // if (node->children[i].triangleIndices.empty() == false)
                // {
                SplitBVHNode(&node->children[i], mesh, depth);
                // }
            }
        }
    }



    // The Game Physcis Cookbook kept this one as a float return for some reason im just changing it
    bool Raycast(const Mesh& mesh, const Ray3& ray, RaycastResult& outResult)
    {
        outResult.Reset();
        // If mesh has no accelerator we simple check every triangle in the mesh in a linear fashion
        if (mesh.accelerator == nullptr)
        {
            for (int i = 0; i < mesh.triangles.size(); i++)
            {
                Raycast(mesh.triangles[i], ray, outResult);
                float result = outResult.t;
                if (result >= 0)
                    return true;
            }
        }
        // Walk through the BVH tree depth first
        else
        {
            std::list<BVHNode*> toProcess;
            toProcess.push_front(mesh.accelerator);
            // Recursively walk the BVH tree
            while (!toProcess.empty())
            {
                // Getting current node:
                BVHNode* iterator = *(toProcess.begin());
                toProcess.erase(toProcess.begin());

                // If node has triangles (aka leaf node) iterate through every triangle
                int currentNumOfTriangles = iterator->triangleIndices.size();
                if (currentNumOfTriangles >= 0)
                {
                    for (int i = 0; i < currentNumOfTriangles; i++)
                    {
                        // Do a raycast against the triangle
                        Raycast(mesh.triangles[iterator->triangleIndices[i]], ray, outResult);
                        float r = outResult.t;

                        if (r >= 0)
                            return true;
                    }
                }

                //if node has children (non leaf) perform a raycast against the bounds of each child
                // If ray hits the bounds of a child add that node to the list of nodes to process
                if (iterator->children != nullptr)
                {
                    for (int i = 8-1; i >= 0; i--)
                    {
                        Raycast(iterator->children[i].bounds, ray, outResult);
                        if (outResult.t >= 0)
                        {
                            toProcess.push_front(&iterator->children[i]);
                        }
                    }
                }
            }
        }

        return false;
    }



    bool IsMeshAABBIntersection(const Mesh& mesh, const AABB& aabb)
    {
        // If mesh has no accelerator we linearly loop through the triangles of the mesh and check for intersection
        if (mesh.accelerator == nullptr)
        {
            for (int i = 0; i < mesh.triangles.size(); i++)
            {
                // NOTE: TriangleAABB test would change if we are testing other shape other than AABB
                if (IsTriangleAABBIntersection(mesh.triangles[i], aabb))
                {
                    return true;
                }
            }
        }
        // Has accelerator so traverse the BVH tree depth first looking for an intersection
        else
        {
            std::list<BVHNode*> toProcess;
            toProcess.push_front(mesh.accelerator);
            while (!toProcess.empty())
            {
                BVHNode* iterator = *(toProcess.begin());
                toProcess.erase(toProcess.begin());

                // If BVH nodes is a leaf node (has triangles)
                int currentNumOfTris = iterator->triangleIndices.size();
                if (currentNumOfTris >= 0)
                {

                    for (int i = 0; i < currentNumOfTris; i++)
                    {
                        // The TriangleAABB test would change if it was other shape and not AABB
                        if (IsTriangleAABBIntersection(mesh.triangles[iterator->triangleIndices[i]], aabb))
                        {
                            return true;
                        }
                    }
                }

                // If BVH node has child nodes (not a leaf) go through each child
                // If bounds (AABB) intersect with shape being tested (this case AABB) we add child node to list of nodes to process
                if (iterator->children != nullptr)
                {
                    for (int i = 8-1; i >= 0; i--)
                    {
                        // AABBAABB test would change if it was testing a shape other than AABB
                        if (IsAABBAABBIntersection(iterator->children[i].bounds, aabb))
                        {
                            toProcess.push_front(&iterator->children[i]);
                        }
                    }
                }
            }
        }

        // if no node and no triangles intersected the AABB being tested
        return false;
    }


    bool IsMeshOBBIntersection(const Mesh& mesh, const OBB& obb)
    {
        // If mesh has no accelerator we linearly loop through the triangles of the mesh and check for intersection
        if (mesh.accelerator == nullptr)
        // if (true)
        {
            for (int i = 0; i < mesh.triangles.size(); i++)
            {
                if (IsTriangleOBBIntersection(mesh.triangles[i], obb))
                {
                    // utility::log("COLLISION");
                    return true;
                }
            }
        }
        // Has accelerator so traverse the BVH tree depth first looking for an intersection
        else
        {
            std::list<BVHNode*> toProcess;
            toProcess.push_front(mesh.accelerator);
            while (!toProcess.empty())
            {
                BVHNode* iterator = *(toProcess.begin());
                toProcess.erase(toProcess.begin());

                // If BVH nodes is a leaf node (has triangles)
                int currentNumOfTris = iterator->triangleIndices.size();
                if (currentNumOfTris >= 0)
                {
                    for (int i = 0; i < currentNumOfTris; i++)
                    {
                        if (IsTriangleOBBIntersection(mesh.triangles[iterator->triangleIndices[i]], obb))
                        {
                            return true;
                        }
                    }
                }

                // If BVH node has child nodes (not a leaf) go through each child
                // If bounds (AABB) intersect with shape being tested (this case OBB) we add child node to list of nodes to process
                if (iterator->children != nullptr)
                {
                    for (int i = 8-1; i >= 0; i--)
                    {
                        // AABBAABB test would change if it was testing a shape other than AABB
                        if (IsAABBOBBIntersection(iterator->children[i].bounds, obb))
                        {
                            toProcess.push_front(&iterator->children[i]);
                        }
                    }
                }
            }
        }

        return false;
    }


    bool IsMeshSphereIntersection(const Mesh& mesh, const Sphere& sphere)
    {
        // If mesh has no accelerator we linearly loop through the triangles of the mesh and check for intersection
        if (mesh.accelerator == nullptr)
        {
            for (int i = 0; i < mesh.triangles.size(); i++)
            {
                if (IsTriangleSphereIntersection(mesh.triangles[i], sphere))
                {
                    return true;
                }
            }
        }
        // Has accelerator so traverse the BVH tree depth first looking for an intersection
        else
        {
            std::list<BVHNode*> toProcess;
            toProcess.push_front(mesh.accelerator);
            while (!toProcess.empty())
            {
                BVHNode* iterator = *(toProcess.begin());
                toProcess.erase(toProcess.begin());

                // If BVH nodes is a leaf node (has triangles)
                int currentNumOfTris = iterator->triangleIndices.size();
                if (currentNumOfTris >= 0)
                {

                    for (int i = 0; i < currentNumOfTris; i++)
                    {
                        if (IsTriangleSphereIntersection(mesh.triangles[iterator->triangleIndices[i]], sphere))
                        {
                            return true;
                        }
                    }
                }

                // If BVH node has child nodes (not a leaf) go through each child
                // If bounds (AABB) intersect with shape being tested (this case OBB) we add child node to list of nodes to process
                if (iterator->children != nullptr)
                {
                    for (int i = 8-1; i >= 0; i--)
                    {
                        // AABBAABB test would change if it was testing a shape other than AABB
                        if (IsSphereAABBIntersection(sphere, iterator->children[i].bounds))
                        {
                            toProcess.push_front(&iterator->children[i]);
                        }
                    }
                }
            }
        }

        return false;
    }


    bool IsMeshPlaneIntersection(const Mesh& mesh, const Plane& plane)
    {
        // If mesh has no accelerator we linearly loop through the triangles of the mesh and check for intersection
        if (mesh.accelerator == nullptr)
        {
            for (int i = 0; i < mesh.triangles.size(); i++)
            {
                if (IsTrianglePlaneIntersection(mesh.triangles[i], plane))
                {
                    return true;
                }
            }
        }
        // Has accelerator so traverse the BVH tree depth first looking for an intersection
        else
        {
            std::list<BVHNode*> toProcess;
            toProcess.push_front(mesh.accelerator);
            while (!toProcess.empty())
            {
                BVHNode* iterator = *(toProcess.begin());
                toProcess.erase(toProcess.begin());

                // If BVH nodes is a leaf node (has triangles)
                int currentNumOfTris = iterator->triangleIndices.size();
                if (currentNumOfTris >= 0)
                {

                    for (int i = 0; i < currentNumOfTris; i++)
                    {
                        if (IsTrianglePlaneIntersection(mesh.triangles[iterator->triangleIndices[i]], plane))
                        {
                            return true;
                        }
                    }
                }

                // If BVH node has child nodes (not a leaf) go through each child
                // If bounds (AABB) intersect with shape being tested (this case OBB) we add child node to list of nodes to process
                if (iterator->children != nullptr)
                {
                    for (int i = 8-1; i >= 0; i--)
                    {
                        // AABBAABB test would change if it was testing a shape other than AABB
                        if (IsAABBPlaneIntersection(iterator->children[i].bounds, plane))
                        {
                            toProcess.push_front(&iterator->children[i]);
                        }
                    }
                }
            }
        }

        return false;
    }


    bool IsMeshTriangleIntersection(const Mesh& mesh, const Triangle& triangle)
    {
        // If mesh has no accelerator we linearly loop through the triangles of the mesh and check for intersection
        if (mesh.accelerator == nullptr)
        {
            for (int i = 0; i < mesh.triangles.size(); i++)
            {
                if (IsTriangleTriangleIntersection(mesh.triangles[i], triangle))
                {
                    return true;
                }
            }
        }
        // Has accelerator so traverse the BVH tree depth first looking for an intersection
        else
        {
            std::list<BVHNode*> toProcess;
            toProcess.push_front(mesh.accelerator);
            while (!toProcess.empty())
            {
                BVHNode* iterator = *(toProcess.begin());
                toProcess.erase(toProcess.begin());

                // If BVH nodes is a leaf node (has triangles)
                int currentNumOfTris = iterator->triangleIndices.size();
                if (currentNumOfTris >= 0)
                {

                    for (int i = 0; i < currentNumOfTris; i++)
                    {
                        if (IsTriangleTriangleIntersection(mesh.triangles[iterator->triangleIndices[i]], triangle))
                        {
                            return true;
                        }
                    }
                }

                // If BVH node has child nodes (not a leaf) go through each child
                // If bounds (AABB) intersect with shape being tested (this case OBB) we add child node to list of nodes to process
                if (iterator->children != nullptr)
                {
                    for (int i = 8-1; i >= 0; i--)
                    {
                        // AABBAABB test would change if it was testing a shape other than AABB
                        if (IsTriangleAABBIntersection(triangle, iterator->children[i].bounds))
                        {
                            toProcess.push_front(&iterator->children[i]);
                        }
                    }
                }
            }
        }

        return false;
    }

    bool IsMeshMeshIntersection(const Mesh& meshA, const Mesh& meshB)
    {
        // If meshA has no accelerator we linearly loop through the triangles of the meshA and check for intersection
        if (meshA.accelerator == nullptr)
        {
            for (int i = 0; i < meshA.triangles.size(); i++)
            {
                if (IsMeshTriangleIntersection(meshB, meshA.triangles[i]))
                {
                    return true;
                }
            }
        }
        // Has accelerator so traverse the BVH tree depth first looking for an intersection
        else
        {
            std::list<BVHNode*> toProcess;
            toProcess.push_front(meshA.accelerator);
            while (!toProcess.empty())
            {
                BVHNode* iterator = *(toProcess.begin());
                toProcess.erase(toProcess.begin());

                // If BVH nodes is a leaf node (has triangles)
                int currentNumOfTris = iterator->triangleIndices.size();
                if (currentNumOfTris >= 0)
                {

                    for (int i = 0; i < currentNumOfTris; i++)
                    {
                        if (IsMeshTriangleIntersection(meshB, meshA.triangles[iterator->triangleIndices[i]]))
                        {
                            return true;
                        }
                    }
                }

                // If BVH node has child nodes (not a leaf) go through each child
                // If bounds (AABB) intersect with shape being tested (this case OBB) we add child node to list of nodes to process
                if (iterator->children != nullptr)
                {
                    for (int i = 8-1; i >= 0; i--)
                    {
                        // AABBAABB test would change if it was testing a shape other than AABB
                        if (IsMeshAABBIntersection(meshB, iterator->children[i].bounds))
                        {
                            toProcess.push_front(&iterator->children[i]);
                        }
                    }
                }
            }
        }

        return false;
    }


    AABB GetTransformedAABB(const Transform3DComponent& transform)
    {
        return AABB(transform.GetPosition(), transform.GetScale());
    }

    void TransformAABB(AABB& aabb, const Transform3DComponent& transformation)
    {
        // Avros method using center and half extents https://stackoverflow.com/a/58630206
        // utility::log("Prev pos: " + aabb.position.GetString());
        Mat4x4 transformMat = transformation.GetWorldMatrix();
        std::array<float, 3> center; 
        std::array<float, 3> halfextents;
        std::array<float, 3> og_center = aabb.position.GetArray();
        std::array<float, 3> og_halfextents = aabb.size.GetArray();

        for (int i = 0; i < 3; i++)
        {
            center[i] = transformMat.m[3][i]; // Translation part
            halfextents[i] = 0.f;
            for (int j = 0; j < 3; j++)
            {
                center[i] += transformMat.m[i][j] * og_center[j];
                halfextents[i] += std::fabsf(transformMat.m[i][j]) * og_halfextents[j];
            }
        }

        aabb.position = Vector3(center[0], center[1], center[2]);
        aabb.size = Vector3(halfextents[0], halfextents[1], halfextents[2]);
        // utility::log("New pos: " + aabb.position.GetString());
    }

    OBB GetTransformedOBB(const Transform3DComponent& transform)
    {
        // Might need to go based on WorldMatrix, although im not too sure...
         // return OBB(obb.position + transform.GetPosition(), obb.size * transform.GetScale(), transform.GetRotationMatrix());
         return OBB(transform.GetPosition(), transform.GetScale(), transform.GetRotationMatrix());
    }

    Plane GetTransformedPlane(const Transform3DComponent& transform)
    {
        // Going to do the triangle transform and then get plane from that
        Triangle tri = GetTransformedTriangle(transform);
        return GetTrianglePlane(tri);
    }

    Sphere GetTransformedSphere(const Transform3DComponent& transform)
    {
        // NOTE: I'm just using the x value for the scale to represent the scaling since it is uniform
        return Sphere(transform.GetPosition(), transform.GetScale().x);
    }

    Triangle GetTransformedTriangle(const Transform3DComponent& transform)
    {
        // Basically doing whats in the DrawMesh method in the GraphicsRenderer but just the transform not the projection
        Triangle triangle = Triangle::YUpTriangle(); // A default triangle
        Triangle triTransformed;
        Mat4x4 matWorld = transform.GetWorldMatrix();

        triTransformed.points[0] = matWorld.GetMultiplyVector3(triangle.points[0]);
        triTransformed.points[1] = matWorld.GetMultiplyVector3(triangle.points[1]);
        triTransformed.points[2] = matWorld.GetMultiplyVector3(triangle.points[2]);

        return triTransformed;
    }


    Mesh GetTransformedMesh(const Mesh& mesh, const Transform3DComponent& transform)
    {
        // Mesh holds all the transformed triangles + the transformed used on them
        // To get the original mesh you would do prevTransform^-1 * mesh
        // Once we get the original mesh we have the original triangles
        // Then we transform the original triangles with new transform
        // So newMesh = original_mesh * transformMat
        //    newMesh = prevTransformMat^-1 * current_mesh * transformMat
        // NOTE: I dont have matrix * mesh, I mean just apply the transform to the mesh
        Mesh result = mesh;
        Mat4x4 matTransform = transform.GetWorldMatrix();
        for (int i = 0; i < mesh.triangles.size(); i++)
        {
            Triangle newTri;
            newTri.points[0] = matTransform.GetMultiplyVector3(mesh.triangles[i].points[0]);
            newTri.points[1] = matTransform.GetMultiplyVector3(mesh.triangles[i].points[1]);
            newTri.points[2] = matTransform.GetMultiplyVector3(mesh.triangles[i].points[2]);

            result.triangles[i] = newTri; 
        }

        result.prevTransform = transform;

        // Updating the BVHNode accelerator (which is same as "OctreeNode" but instead of storing entities it stores triangles)
        if (result.accelerator != nullptr)
        {
            // UpdateBVHNodeTransform(mesh.accelerator, transform, mesh.prevTransform);
            delete result.accelerator;
            result.accelerator = nullptr;
            AccelerateMesh(result); // Creates the BVH tree
        }
        else
        {
            AccelerateMesh(result); // Creates the BVH tree
        }

        return result;
    }

    void TransformMesh(Mesh& mesh, const Transform3DComponent& transform)
    {
        // Mesh holds all the transformed triangles + the transformed used on them
        // To get the original mesh you would do prevTransform^-1 * mesh
        // Once we get the original mesh we have the original triangles
        // Then we transform the original triangles with new transform
        // So newMesh = original_mesh * transformMat
        //    newMesh = prevTransformMat^-1 * current_mesh * transformMat
        // NOTE: I dont have matrix * mesh, I mean just apply the transform to the mesh

        Mat4x4 matTransform = transform.GetWorldMatrix() * mesh.prevTransform.GetWorldMatrix().GetInverse();
        for (int i = 0; i < mesh.triangles.size(); i++)
        {
            Triangle newTri;
            newTri.points[0] = matTransform.GetMultiplyVector3(mesh.triangles[i].points[0]);
            newTri.points[1] = matTransform.GetMultiplyVector3(mesh.triangles[i].points[1]);
            newTri.points[2] = matTransform.GetMultiplyVector3(mesh.triangles[i].points[2]);

            mesh.triangles[i] = newTri; 
        }

        mesh.prevTransform = transform;

        // Updating the BVHNode accelerator (which is same as "OctreeNode" but instead of storing entities it stores triangles)
        if (mesh.accelerator != nullptr)
        {
            // UpdateBVHNodeTransform(mesh.accelerator, transform, mesh.prevTransform);
            delete mesh.accelerator;
            mesh.accelerator = nullptr;
            AccelerateMesh(mesh); // Creates the BVH tree
        }
        else
        {
            AccelerateMesh(mesh); // Creates the BVH tree
        }
    }


    CollisionManifold FindCollisionManifold(const Sphere& sphereA, const Sphere& sphereB)
    {
        CollisionManifold result;

        // combined radius of 2 spheres
        float r = sphereA.radius + sphereB.radius;

        // Distance between 2 spheres
        Vector3 d = sphereB.position - sphereA.position;

        // Use d as the direction from B to A
        d.Normalize();

        // We know they intersect since a manifold implies an intersection
        result.isColliding = true;
        result.normal = d;
        result.depth = fabsf(d.GetMagnitude() - r) * 0.5f;
        // dtp = distance to intersection point
        float dtp = sphereA.radius - result.depth;
        Point3 contact = sphereA.position + d * dtp;
        result.contacts.push_back(contact);

        return result;
    }


    CollisionManifold FindCollisionManifold(const OBB& obbA, const Sphere& sphereB)
    {
        CollisionManifold result;

        // Find closest point to the center of the sphere on the OBB
        Point3 closestPoint = GetClosestPoint(obbA, sphereB.position);
        
        float distanceSq = (closestPoint - sphereB.position).GetMagnitudeSquared();

        // If the closest point is at the center of the sphere we can't easily build a collision normal
        // if thats the case we try to find a new closest point
        Vector3 normal;
        if (CMP_FLOAT(distanceSq, 0.f))
        {
            float mSq = (closestPoint - obbA.position).GetMagnitudeSquared();
            if (CMP_FLOAT(mSq, 0.f))
                return result;

            // Closest point is at the center of the spehere
            normal = (closestPoint - obbA.position).GetNormal();
        }
        else
        {
            normal = (sphereB.position - closestPoint).GetNormal();
        }

        Point3 outsidePoint = sphereB.position - normal * sphereB.radius;
        float distance = (closestPoint - outsidePoint).GetMagnitude();

        result.isColliding = true;
        result.contacts.push_back(closestPoint + (outsidePoint - closestPoint) * 0.5f);
        result.normal = normal;
        result.depth = distance * 0.5f;

        return result;
    }


    CollisionManifold FindCollisionManifold(const Sphere& sphereA, const OBB& obbB)
    {
        CollisionManifold result = FindCollisionManifold(obbB, sphereA);

        // Need to reverse normal
        result.normal = result.normal * -1.f;
        return result;
    }


    std::array<Line3, 12> GetEdges(const OBB& obb)
    {
        std::array<Line3, 12> result;

        std::array<Point3, 8> vertices = obb.GetVertices();

        // Every element in this array is an edge between vertices specified by index
        int index[][2] = {
            {6,1}, {6,3}, {6,4}, {2,7}, {2,5}, {2,0},
            {0,1}, {0,3}, {7,1}, {7,4}, {4,5}, {5,3}
        };

        // Loops through index and constructs edges from the vertex pairs
        for (int j = 0; j < 12; j++)
        {
            result[j] = Line3(vertices[index[j][0]], vertices[index[j][1]]);
        }

        return result;
    }


    std::array<Plane, 6> GetPlanes(const OBB& obb)
    {
        Vector3 c = obb.position; // OBB Center
        Vector3 e = obb.size; // OBB Extents
                              //
        std::array<Vector3, 3> a = obb.orientation.GetVector3Array(); // OBB Axis

        // OBB is made of 6 planes (each face)
        std::array<Plane, 6> result;

        // Construct a plane for every face using a point on each face and normal of each face
        result[0] = Plane(a[0], (a[0].GetDotProduct(c + a[0] * e.x)));
        result[1] = Plane(a[0] * -1.f, -1.f * (a[0].GetDotProduct(c - a[0] * e.x)));
        result[2] = Plane(a[1], (a[1].GetDotProduct(c + a[1] * e.y)));
        result[3] = Plane(a[1] * -1.f, -1.f * (a[1].GetDotProduct(c - a[1] * e.y)));
        result[4] = Plane(a[2], (a[2].GetDotProduct(c + a[2] * e.z)));
        result[5] = Plane(a[2] * -1.f, -1.f * (a[2].GetDotProduct(c - a[2] * e.z)));

        return result;
    }


    // Checks if a line intersects a plane and if it does, the line is clipped to the plane (just a helper function)
    bool ClipToPlane(const Plane& plane, const Line3& line, Point3* outPoint)
    {
        // Ensuring that the line and plane intersect
        Vector3 ab = line.end - line.start;
        float nAB = plane.normal.GetDotProduct(ab);
        if (CMP_FLOAT(nAB, 0.f))
            return 0;

        // Finding time along the line at which it intersects the plane
        float nA = plane.normal.GetDotProduct(line.start);
        float t = (plane.distance - nA) / nAB;

        // If time was valid return the point at which the line and plane intersect
        if (t >= 0.f && t <= 1.f)
        {
            if (outPoint != nullptr)
            {
                *outPoint = line.start + ab * t;
            }
            return true;
        }

        // If time is not within the range of 0-1, the plane and line segment do not intersect
        // The plane might intersect an infinite line, but we only care about the segment
        return false;
    }


    // Takes a list of edges that represent an oob and another oob
    // edges provided are clipped against the planes of the provided bounding box
    std::vector<Point3> ClipEdgesToOBB(const std::vector<Line3>& edges, const OBB& obb)
    {
        // Will have at most as many output points as we had input edges
        std::vector<Point3> result;
        result.reserve(edges.size());
        Point3 intersection;

        std::array<Plane, 6> planes = GetPlanes(obb);
        for (int i = 0; i < 6; i++)
        {
            // For each plane loop through each provided edge
            for (int j = 0; j < edges.size(); j++)
            {
                // Try to clip the current edge to the current plane
                if (ClipToPlane(planes[i], edges[j], &intersection))
                {
                    if (IsPointInOBB(intersection, obb))
                        result.push_back(intersection);
                }
            }
        }

        return result;
    }


    // Similar logic to testing if objs separate on a single axis in the SAT test
    float PenetrationDepth(const OBB& o1, const OBB& o2, const Vector3& axis, bool* outShouldFlip)
    {
        // Project both the OBB's onto the provided axis and store their respective intervals
        Interval i1 = GetInterval(o1, axis.GetNormal());
        Interval i2 = GetInterval(o2, axis.GetNormal());

        // If the intervals dont overlap there is no penetration
        if (!((i2.min <= i1.max) && (i1.min <= i2.max)))
            return 0.f; // No penetration

        float len1 = i1.max - i1.min;
        float len2 = i2.max - i2.min;

        float min = fminf(i1.min, i2.min);
        float max = fmaxf(i1.max, i2.max);

        // Length of the combined intervals
        float length = max - min;

        // Depending on order of args, we might need to flip the collision normal outside this func
        // If 2nd obb is in front of the first one, the collision normal will be flipped
        if (outShouldFlip != nullptr)
        {
            *outShouldFlip = (i2.min < i1.min);
        }

        // returning length of the intersection (penetration)
        return (len1 + len2) - length;
    }

    CollisionManifold FindCollisionManifold(const OBB& obbA, const OBB& obbB)
    {
        CollisionManifold result;

        std::array<Vector3, 3> o1 = obbA.orientation.GetVector3Array();
        std::array<Vector3, 3> o2 = obbB.orientation.GetVector3Array();

        // SAT test
        // Face axes
        std::array<Vector3, 15> test = {
            o1[0],
            o1[1],
            o1[2],
            o2[0],
            o2[1],
            o2[2]
        };

        // Rest of axes
        for (int i = 0; i < 3; i++)
        {
            // test[6 + i * 3 + 0] = test[i].GetCrossProduct(test[0]);
            // test[6 + i * 3 + 1] = test[i].GetCrossProduct(test[1]);
            // test[6 + i * 3 + 2] = test[i].GetCrossProduct(test[2]);
            // ROBUST
            test[6 + i * 3 + 0] = GetSATAxisRobust(test[i], test[0]);
            test[6 + i * 3 + 1] = GetSATAxisRobust(test[i], test[1]);
            test[6 + i * 3 + 2] = GetSATAxisRobust(test[i], test[2]);
        }

        // Create a temp var for the dir of the collision normal
        Vector3* hitNormal = nullptr;
        bool shouldFlip;

        // test 15 axes of potential separation for intersection
        for (int i = 0; i < 15; i++)
        {
            // if (test[i].GetMagnitudeSquared() < 0.001f)
            //     continue;

            float depth = PenetrationDepth(obbA, obbB, test[i], &shouldFlip);

            if (depth <= 0.f)
            {
                return result; // No intersection
            }
            else if (depth < result.depth)
            {
                if (shouldFlip)
                {
                    test[i] = test[i] * -1.f;
                }

                // Storing depth and collision normal
                result.depth = depth;
                hitNormal = &test[i];
            }
        }

        if (hitNormal == nullptr)
            return result; // no intersection
        
        Vector3 axis = hitNormal->GetNormal();

        // Clip each obb against the other, leaves us with a list of intersection points
        std::array<Line3, 12> edgesA = GetEdges(obbA);
        std::array<Line3, 12> edgesB = GetEdges(obbB);
        std::vector<Line3> vEdgesA(edgesA.begin(), edgesA.end());
        std::vector<Line3> vEdgesB(edgesB.begin(), edgesB.end());

        std::vector<Point3> c1 = ClipEdgesToOBB(vEdgesB, obbA);
        std::vector<Point3> c2 = ClipEdgesToOBB(vEdgesA, obbB);
        result.contacts.reserve(c1.size() + c2.size());
        result.contacts.insert(result.contacts.end(), c1.begin(), c1.end());
        result.contacts.insert(result.contacts.end(), c2.begin(), c2.end());

        // Project the result of the clipped points onto a shared plane
        // shared plane is constructed out of the collision normal
        Interval i = GetInterval(obbA, axis);
        float distance = (i.max - i.min) * 0.5f - result.depth * 0.5f;
        Vector3 pointOnPlane = obbA.position + axis * distance;

        for (int i = result.contacts.size() - 1; i >= 0; i--)
        {
            Vector3 contact = result.contacts[i];
            // Store result of projection
    
            result.contacts[i] = contact + (axis * axis.GetDotProduct(pointOnPlane - contact));

            // Removing duplicate points
            for (int j = result.contacts.size() - 1; j > i; j--)
            {
                if ((result.contacts[j] - result.contacts[i]).GetMagnitudeSquared() < 0.0001f)
                {
                    result.contacts.erase(result.contacts.begin() + j);
                    break;
                }
            }
        }

        result.isColliding = true;
        result.normal = axis;

        return result;
    }


    // An OBB without rotation.
    OBB CreateOBBFromAABB(const AABB& aabb)
    {
        // The orientation by default is set to identity
        return OBB(aabb.position, aabb.size);
    }

    Vector3 GetSATAxisRobust(const Vector3& axisA, const Vector3& axisB)
    {
        Vector3 result = axisA.GetCrossProduct(axisB);

        // Cross prod != 0 therefore not parallel
        if (!CMP_FLOAT(result.GetMagnitudeSquared(), 0.f))
        {
            return result;
        }
        else // Are parallel
        {
            // Construct temporary axis perpendicular to axisA
            Vector3 temp = axisA.GetPerpendicularVector();
            result = axisA.GetCrossProduct(temp);

            if (!CMP_FLOAT(result.GetMagnitudeSquared(), 0.f))
                return result; // Not parallel
            else
                return Vector3(0, 0, 0); // No way to get proper cross product
        }
    }

    Mesh GetDefaultPrimitiveMesh(ShapeType shape)
    {
        Mesh mesh;

        switch (shape)
        {
            case ShapeType::AABB_3D:
                mesh.LoadFromObjectFile("./data/EngineObjects/cube.obj");
                break;
            case ShapeType::OBB_3D:
                mesh.LoadFromObjectFile("./data/EngineObjects/cube.obj");
                break;
            case ShapeType::PLANE_3D:
                mesh.LoadFromObjectFile("./data/EngineObjects/plane.obj");
                break;
            case ShapeType::SPHERE_3D:
                mesh.LoadFromObjectFile("./data/EngineObjects/sphere.obj");
                break;
            case ShapeType::TRIANGLE_3D:
                mesh.triangles.push_back(Triangle::YUpTriangle());
                break;
            default:
                break;
        }

        return mesh;
    }

}
