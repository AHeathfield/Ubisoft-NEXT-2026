#pragma once

#include "../Math/Geometry/3D/AABB.h"
#include "../Math/Geometry3D.h"
#include "../Math/ShapeType.h"
#include "../Physics/CollisionManifold.h"
#include "../Core/ECS.h"
#include "../../config.h"
#include "Transform3DComponent.h"
#include <unordered_set>
#include <vector>

enum class ColliderType
{
	STATIC,     // Colliders that don't move
	DYNAMIC,    // Colliders that can move
	//TRIGGER   // NO NEED
};

struct ColliderComponent
{
    ColliderType colliderType = ColliderType::STATIC;
    ShapeType shape = ShapeType::AABB_3D;

    // Union can only be 1 of these values
    union Collider
    {
        AABB aabb;
        OBB obb;
        Plane plane;
        Sphere sphere;
        Triangle triangle;
    } collider = {.aabb = AABB()}; // Default

    // If it is a meshCollider I want it to be separate from the render mesh so transforms applied don't effect the render
    Mesh meshColliderOriginal = Mesh();
    Mesh meshCollider = Mesh(); // can't be in union due to using std::vector!
#if DEBUG_MODE
    Mesh meshRender = Mesh();
#endif

    bool isActive = true;

    // To apply physics if any
    std::function<void(Entity self, Entity other, const CollisionManifold& collision)> physicsResponse = nullptr;

    // Custom collision functions per entity
    std::function<void(Entity self, Entity other, const CollisionManifold& collision)> onCollisionEnter = nullptr;
    std::function<void(Entity self, Entity other, const CollisionManifold& collision)> onCollisionStay = nullptr;
    std::function<void(Entity self, Entity other)> onCollisionExit = nullptr;
    std::unordered_set<Entity> currentCollisions;




    // Constructors
    ColliderComponent() = default;
    ColliderComponent(ColliderType type, ShapeType collider_shape, bool active = true)
	: colliderType(type), shape(collider_shape), isActive(active)
    {
	switch (collider_shape)
	{
	    case ShapeType::AABB_3D:
            collider.aabb = AABB();
            break;
	    case ShapeType::OBB_3D:
            collider.obb = OBB();
            break;
	    case ShapeType::PLANE_3D:
            collider.plane = Plane();
            break;
	    case ShapeType::SPHERE_3D:
            collider.sphere = Sphere();
            break;
	    case ShapeType::TRIANGLE_3D:
            collider.triangle = Triangle::YUpTriangle();
            break;
	    case ShapeType::MESH_3D:
            meshColliderOriginal = Mesh();
            meshCollider = Mesh();
            break;
	}

#if DEBUG_MODE
	meshRender = Geometry3D::GetDefaultPrimitiveMesh(collider_shape);
	meshRender.isWireframe = true;
	meshRender.color = Color(255, 255, 255);
#endif
    }
    ColliderComponent(ColliderType type, const Mesh& mesh,  bool active = true)
	: colliderType(type),
	shape(ShapeType::MESH_3D),
	meshColliderOriginal(mesh),
	meshCollider(mesh),
	isActive(active)
    {
#if DEBUG_MODE
	meshRender = mesh;
	meshRender.isWireframe = true;
	meshRender.color = Color(255, 255, 255);
#endif
    }
    ColliderComponent(ColliderType type, ShapeType collider_shape, std::function<void(Entity self, Entity other, const CollisionManifold& collision)> collision_enter_response, std::function<void(Entity self, Entity other, const CollisionManifold& collision)> collision_stay_response, std::function<void(Entity self, Entity other)> collision_exit_response, std::function<void(Entity self, Entity other, const CollisionManifold& collision)> physics_response = nullptr, bool active = true)
	: ColliderComponent(type, collider_shape, active)
    {
	physicsResponse = physics_response;
	onCollisionEnter = collision_enter_response;
	onCollisionStay = collision_stay_response;
	onCollisionExit = collision_exit_response;
    }
    ColliderComponent(ColliderType type, const Mesh& mesh, std::function<void(Entity self, Entity other, const CollisionManifold& collision)> collision_enter_response, std::function<void(Entity self, Entity other, const CollisionManifold& collision)> collision_stay_response, std::function<void(Entity self, Entity other)> collision_exit_response, std::function<void(Entity self, Entity other, const CollisionManifold& collision)> physics_response = nullptr, bool active = true)
	: ColliderComponent(type, mesh, active)
    {
	physicsResponse = physics_response;
	onCollisionEnter = collision_enter_response;
	onCollisionStay = collision_stay_response;
	onCollisionExit = collision_exit_response;
    }





    void UpdateWorldSpace(const Transform3DComponent& transform)
    {
        switch (shape)
        {
            case ShapeType::AABB_3D:
                collider.aabb = Geometry3D::GetTransformedAABB(transform);
                break;
            case ShapeType::OBB_3D:
                collider.obb = Geometry3D::GetTransformedOBB(transform);
                break;
            case ShapeType::PLANE_3D:
                collider.plane = Geometry3D::GetTransformedPlane(transform);
                break;
            case ShapeType::SPHERE_3D:
                collider.sphere = Geometry3D::GetTransformedSphere(transform);
                break;
            case ShapeType::TRIANGLE_3D:
                collider.triangle = Geometry3D::GetTransformedTriangle(transform);
                break;
            case ShapeType::MESH_3D:
                // Transform will create a BVH tree for mesh if it doesn't have one
                meshCollider = Geometry3D::GetTransformedMesh(meshColliderOriginal, transform);
        }

#if DEBUG_MODE
	// Geometry3D::TransformMesh(meshRender, transform);
#endif
    }
};
