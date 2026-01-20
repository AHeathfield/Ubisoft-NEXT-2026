#pragma once

#include "../Core/ECS.h"
#include "../Math/OctreeNode.h"
#include "../Math/ShapeType.h"
#include "../Components/ColliderComponent.h"
#include <unordered_map>
#include <unordered_set>


// Handles detecting and responding to collisions
class CollisionSystem : public System
{
public:
    void Init() override;
    void Update(float deltaTime) override;
    void OnEntityAdded(Entity entity) override;
    void OnEntityRemoved(Entity entity) override;
    void Close() override;
    bool CreateBVHOctree(OctreeNode*& root, const std::unordered_set<Entity>& entities, const Vector3& pos, float size); // An Acceleration method

    CollisionManifold GetClosestStaticManifold(const ColliderComponent& collider);
    CollisionManifold GetClosestDynamicManifold(const ColliderComponent& collider);

    std::unordered_set<Entity> QueryStaticCollider(const ColliderComponent& collider);
    std::unordered_set<Entity> QueryDynamicCollider(const ColliderComponent& collider);

    // This is if you want to see all the entities the ray hit and not just the first one
    std::unordered_set<Entity> QueryStaticRay(const Ray3&ray, const float rayLength);
    std::unordered_set<Entity> QueryDynamicRay(const Ray3&ray, const float rayLength);

    // A raycast against a collider component
    bool RaycastCollider(const Ray3& ray, RaycastResult& outResult, const ColliderComponent& colliderComp);

    // Gets closest static object raycast hit
    bool RaycastStatic(const Ray3& ray, RaycastResult& outResult, const float rayLength);
    bool RaycastDynamic(const Ray3& ray, RaycastResult& outResult, const float rayLength);


private:
    // Octree helper methods (MIGHT NEED TO BE PUBLIC)
    void splitOctree(OctreeNode* node, int depth);
    void insertOctreeNode(OctreeNode* node, Entity entity);
    void removeOctreeNode(OctreeNode* node, Entity entity);
    void updateOctreeNode(OctreeNode* node, Entity entity);
    std::unordered_set<Entity> queryOctree(OctreeNode* node, const ColliderComponent& collider);
    std::unordered_set<Entity> queryRayOctree(OctreeNode* node, const Ray3& ray, const float rayLength);

    RaycastResult getClosestRaycast(const Ray3& ray, const float rayLength, const std::unordered_set<Entity>& ents_to_check);

    void transformColliders();
    void checkCollisions();
    bool isCollision(const ColliderComponent& colA, const ColliderComponent& colB);

    CollisionManifold getManifold(const ColliderComponent& colA, const ColliderComponent& colB);
    CollisionManifold getClosestManifold(const ColliderComponent& collider, const std::unordered_set<Entity> ents_to_check);

    void solveConstraints(); // This is more of a physics thing
    void handleCollisions(); // This calls the OnCollision method of the collider (more specific collision)

    // Helper functions for collision detection
    static bool AABB_AABB(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool AABB_OBB(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool AABB_PLANE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool AABB_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool AABB_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool AABB_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool OBB_OBB(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool OBB_AABB(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool OBB_PLANE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool OBB_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool OBB_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool OBB_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool PLANE_PLANE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool PLANE_AABB(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool PLANE_OBB(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool PLANE_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool PLANE_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool PLANE_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool SPHERE_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool SPHERE_AABB(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool SPHERE_OBB(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool SPHERE_PLANE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool SPHERE_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool SPHERE_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool TRIANGLE_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool TRIANGLE_AABB(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool TRIANGLE_OBB(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool TRIANGLE_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool TRIANGLE_PLANE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool TRIANGLE_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool MESH_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool MESH_AABB(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool MESH_OBB(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool MESH_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool MESH_PLANE(const ColliderComponent& col1, const ColliderComponent& col2);
    static bool MESH_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);


    // TODO: finish commented ones
    // Helper functions for collision response
    static CollisionManifold man_AABB_AABB(const ColliderComponent& col1, const ColliderComponent& col2);
    static CollisionManifold man_AABB_OBB(const ColliderComponent& col1, const ColliderComponent& col2);
    // static CollisionManifold man_AABB_PLANE(const ColliderComponent& col1, const ColliderComponent& col2);
    static CollisionManifold man_AABB_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2);
    // static CollisionManifold man_AABB_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);
    // static CollisionManifold man_AABB_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
    static CollisionManifold man_OBB_OBB(const ColliderComponent& col1, const ColliderComponent& col2);
    static CollisionManifold man_OBB_AABB(const ColliderComponent& col1, const ColliderComponent& col2);
    // static CollisionManifold man_OBB_PLANE(const ColliderComponent& col1, const ColliderComponent& col2);
    static CollisionManifold man_OBB_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2);
    // static CollisionManifold man_OBB_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);
    // static CollisionManifold man_OBB_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
    // static CollisionManifold man_PLANE_PLANE(const ColliderComponent& col1, const ColliderComponent& col2);
    // static CollisionManifold man_PLANE_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2);
    // static CollisionManifold man_PLANE_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);
    // static CollisionManifold man_PLANE_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
    static CollisionManifold man_SPHERE_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2);
    static CollisionManifold man_SPHERE_AABB(const ColliderComponent& col1, const ColliderComponent& col2);
    static CollisionManifold man_SPHERE_OBB(const ColliderComponent& col1, const ColliderComponent& col2);
    // static CollisionManifold man_SPHERE_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);
    // static CollisionManifold man_SPHERE_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
    // static CollisionManifold man_TRIANGLE_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);
    // static CollisionManifold man_TRIANGLE_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
    // static CollisionManifold man_MESH_MESH(const ColliderComponent& col1, const ColliderComponent& col2);



private:
    struct ExitingPair
    {
        Entity entA;
        Entity entB;
    };

    // This notation is a function pointer with parameters ColliderComponent*
    using CollisionFunc = bool(*)(const ColliderComponent&, const ColliderComponent&);
    using ManifoldFunc = CollisionManifold(*)(const ColliderComponent&, const ColliderComponent&);
    using RaycastFunc = bool(*)(const Ray3&, const ColliderComponent&);

    // This is a dispatch table pointing to collision functions
    CollisionFunc mCollisionTable[6][6];
    ManifoldFunc mManifoldTable[6][6];

    std::unordered_set<Entity> mStaticEntities;
    std::unordered_set<Entity> mDynamicEntities;

    // In the game physics textbook they have it setup so their scenes may or may not have an octree setup, I'm going to just always set one up since this is a collision system and not a scene
    OctreeNode* mStaticOctree; // Only updated when entities are added (with colliders)
    OctreeNode* mDynamicOctree; // Checks for updates every frame and updates whenever a transform changes

    std::unordered_set<Entity> mDeleted;
    std::unordered_map<Entity, std::unordered_set<Entity>> mCollisions;
    std::vector<ExitingPair> mExitings;
};
