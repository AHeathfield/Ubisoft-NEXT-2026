#pragma once

#include "Geometry/3D/AABB.h"
#include "Geometry/3D/OBB.h"
#include "Geometry/3D/Line3.h"
#include "Geometry/3D/Plane.h"
#include "Geometry/3D/Point3.h"
#include "Geometry/3D/Ray3.h"
#include "Geometry/3D/Sphere.h"
#include "ShapeType.h"
#include "Triangle.h"
#include "../Graphics/Mesh.h"
#include "BVHNode.h"
#include "../Components/Transform3DComponent.h"
#include "../Physics/CollisionManifold.h"
#include "Vector3.h"
#include <array>

// This is a namespace that contains methods regarding interactions between the various 3D Geometry defined
namespace Geometry3D
{
    bool IsPointInSphere(const Point3& point, const Sphere& sphere);
    bool IsPointInAABB(const Point3& point, const AABB& aabb);
    bool IsPointInOBB(const Point3& point, const OBB& obb);
    bool IsPointOnPlane(const Point3& point, const Plane& plane);
    bool IsPointOnLine(const Point3& point, const Line3& line);
    bool IsPointOnRay(const Point3& point, const Ray3& ray);

    // Finds the closest point on geometry from another point
    Point3 GetClosestPoint(const Sphere& sphere, const Point3& point);
    Point3 GetClosestPoint(const AABB& aabb, const Point3& point);
    Point3 GetClosestPoint(const OBB& obb, const Point3& point);
    Point3 GetClosestPoint(const Plane& plane, const Point3& point);
    Point3 GetClosestPoint(const Line3& line, const Point3& point);
    Point3 GetClosestPoint(const Ray3& ray, const Point3& point);
    Point3 GetClosestPoint(const Triangle& triangle, const Point3& point);


    // ================ Shape-Shape Intersections ==================
    bool IsSphereSphereIntersection(const Sphere& s1, const Sphere& s2);
    bool IsSphereAABBIntersection(const Sphere& sphere, const AABB& aabb);
    bool IsSphereOBBIntersection(const Sphere& sphere, const OBB& obb);
    bool IsSpherePlaneIntersection(const Sphere& sphere, const Plane& plane);


    // This is to help with AABB and OBB collisions
    struct Interval
    {
        float min;
        float max;
    };
    Interval GetInterval(const AABB& aabb, const Vector3& axis);
    Interval GetInterval(const OBB& obb, const Vector3& axis);
    bool IsOverlapOnAxis(const AABB& aabb, const OBB& obb, const Vector3& axis);
    bool IsOverlapOnAxis(const OBB& obb1, const OBB& obb2, const Vector3& axis);

    bool IsAABBAABBIntersection(const AABB& aabb1, const AABB& aabb2);
    bool IsAABBOBBIntersection(const AABB& aabb, const OBB& obb);
    bool IsAABBPlaneIntersection(const AABB& aabb, const Plane& plane);

    bool IsOBBOBBIntersection(const OBB& obb1, const OBB& obb2);
    bool IsOBBPlaneIntersection(const OBB& obb, const Plane& plane);

    bool IsPlanePlaneIntersection(const Plane& plane1, const Plane& plane2);
    // =======================================================


    // ================ Line/Ray Intersections ==================
    // With Raycasts, in Godot they are equilvalent to just "Lines" with an origin and target position
    // In this engine a ray has an origin and a direction, we need to see how much time it takes before the ray hits something so we assume that it takes 1 second to travel 1 unit along ray
    // Another way to think of t is the distance from the ray to first collision point
    // If it does not intersect t is undefined which we represent with a negative number
    bool Raycast(const Sphere& sphere, const Ray3& ray, RaycastResult& outResult);
    bool Raycast(const AABB& aabb, const Ray3& ray, RaycastResult& outResult);
    bool Raycast(const OBB& obb, const Ray3& ray, RaycastResult& outResult);
    bool Raycast(const Plane& plane, const Ray3& ray, RaycastResult& outResult);
    bool Raycast(const Triangle& triangle, const Ray3& ray, RaycastResult& outResult);
    bool Raycast(const Mesh& mesh, const Ray3& ray, RaycastResult& outResult);

    bool IsLineIntersection(const Sphere& sphere, const Line3& line);
    bool IsLineIntersection(const AABB& aabb, const Line3& line);
    bool IsLineIntersection(const OBB& obb, const Line3& line);
    bool IsLineIntersection(const Plane& plane, const Line3& line);
    bool IsLineIntersection(const Triangle& triangle, const Line3& line);
    // ==========================================================



    // ================ Triangle and Mesh Intersections ==================
    // NOTE: For meshes in a 3D model we can only test for intersection not containment
    // Containment will require something called a convex hull
    bool IsPointInTriangle(const Point3& p, const Triangle& t);
    Plane GetTrianglePlane(const Triangle& t);
    bool IsTriangleSphereIntersection(const Triangle& t, const Sphere& s);
    Interval GetInterval(const Triangle& triangle, const Vector3& axis);
    bool IsOverlapOnAxis(const AABB& aabb, const Triangle& triangle, const Vector3& axis);
    bool IsTriangleAABBIntersection(const Triangle& t, const AABB& aabb);
    bool IsOverlapOnAxis(const OBB& obb, const Triangle& triangle, const Vector3& axis);
    bool IsTriangleOBBIntersection(const Triangle& t, const OBB& obb);
    bool IsTrianglePlaneIntersection(const Triangle& t, const Plane& p);
    bool IsOverlapOnAxis(const Triangle& t1, const Triangle& t2, const Vector3& axis);
    bool IsTriangleTriangleIntersection(const Triangle& t1, const Triangle& t2);

    // This is a more robust SAT test, it will detect if the cross product of 2 edges is 0
    // If it is the function will use an axis perpendicular to the first edge to try to get a new axis
    // If no such test axis exists, the the 2 edges being tested must be on a line (return 0 vector)
    // A - Edge / Triangle 0, Point 0
    // B - Edge / Triangle 0, Point 1
    // C - Edge / Triangle 1, Point 0
    // D - Edge / Triangle 1, Point 1
    // TODO: Implement this SAT test into the other methods that rely on SAT test
    Vector3 GetSATCrossEdge(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d);
    bool IsTriangleTriangleIntersectionRobust(const Triangle& t1, const Triangle& t2);

    // Returns the Barycentric coordinates of a point with respect to a triangle
    Vector3 GetBarycentric(const Point3& p, const Triangle& t);

    // Creates the accelerator (BVHNode) in the mesh object
    void AccelerateMesh(Mesh& mesh);
    void SplitBVHNode(BVHNode* node, const Mesh& mesh, int depth);


    bool IsMeshAABBIntersection(const Mesh& mesh, const AABB& aabb);
    bool IsMeshOBBIntersection(const Mesh& mesh, const OBB& obb);
    bool IsMeshSphereIntersection(const Mesh& mesh, const Sphere& sphere);
    bool IsMeshPlaneIntersection(const Mesh& mesh, const Plane& plane);
    bool IsMeshTriangleIntersection(const Mesh& mesh, const Triangle& triangle);
    bool IsMeshMeshIntersection(const Mesh& meshA, const Mesh& meshB);

    
    // ================= Shape Transforms ================
    // "transformation" is how much its going to change by that method call
    // Changed to just transform a default primitive instead of applying a tranformation on current!
    AABB GetTransformedAABB(const Transform3DComponent& transform);
    void TransformAABB(AABB& aabb, const Transform3DComponent& transformation);
    OBB GetTransformedOBB(const Transform3DComponent& transform);
    Plane GetTransformedPlane(const Transform3DComponent& transform);
    Sphere GetTransformedSphere(const Transform3DComponent& transform);
    Triangle GetTransformedTriangle(const Transform3DComponent& transform);
    Mesh GetTransformedMesh(const Mesh& mesh, const Transform3DComponent &transform);

    // This still just transforms original but just doing it directly since meshes can be big
    void TransformMesh(Mesh& mesh, const Transform3DComponent& transform);


    // ================= Collision Manifolds ================
    CollisionManifold FindCollisionManifold(const Sphere& sphereA, const Sphere& sphereB);
    CollisionManifold FindCollisionManifold(const OBB& obbA, const Sphere& sphereB);
    CollisionManifold FindCollisionManifold(const Sphere& sphereA, const OBB& obbB);
    CollisionManifold FindCollisionManifold(const OBB& obbA, const OBB& obbB);

    // Helpers
    Vector3 GetSATAxisRobust(const Vector3& axisA, const Vector3& axisB); // This is a general SAT robust test testing the edge cases
    // The GetSATCrossEdge is if you have 2 edges which is more specific for triangles
    // std::array<Point3, 8> GetVertices(const OBB& obb);
    std::array<Line3, 12> GetEdges(const OBB& obb);
    std::array<Plane, 6> GetPlanes(const OBB& obb);
    bool ClipToPlane(const Plane& plane, const Line3& line, Point3* outPoint);
    std::vector<Point3> ClipEdgesToOBB(const std::vector<Line3>& edges, const OBB& obb);
    float PenetrationDepth(const OBB& o1, const OBB& o2, const Vector3& axis, bool* outShouldFlip);



    // ======================= OTHER ==================
    OBB CreateOBBFromAABB(const AABB& aabb);
    Mesh GetDefaultPrimitiveMesh(ShapeType shape);
}
